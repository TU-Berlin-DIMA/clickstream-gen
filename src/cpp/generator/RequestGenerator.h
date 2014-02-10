/*
 * Copyright 2010-2011 DIMA Research Group, TU Berlin
 * 
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * 
 *     http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 * 
 * @author: Alexander Alexandrov <alexander.alexandrov@tu-berlin.de>
 */

#ifndef REQUESTGENERATOR_H_
#define REQUESTGENERATOR_H_

#include "generator/RandomSetGenerator.h"
#include "generator/SessionGenerator.h"
#include "hydrator/ClusterIntervalHydrator.h"
#include "hydrator/ConstrainedReferenceHydrator.h"
#include "hydrator/RangeSetHydrator.h"
#include "record/Request.h"

using namespace Myriad;
using namespace Poco;

namespace ClickStreamGen {

class RequestHydratorChain;

class RequestGenerator: public RandomSetGenerator<Request>
{
public:

	typedef RecordTraits<Request>::HydratorChainType HydratorChainType;

	RequestGenerator(const string& name, GeneratorConfig& config, NotificationCenter& notificationCenter) :
		RandomSetGenerator<Request>(name, config, notificationCenter)
	{
	}

	void prepare(Stage stage, const GeneratorPool& pool);

	HydratorChainType hydratorChain(BaseHydratorChain::OperationMode opMode, RandomStream& random);
};

/**
 * Hydrator specialization for Request.
 */
class RequestHydratorChain : public HydratorChain<Request>
{
public:

	// hydrator type aliases
	typedef RangeSetHydrator<Request, String> ServerHydrator;
	typedef EnumSetHydrator<Request, ProductCategory> ProductCategoryHydrator;
	typedef ClusterIntervalHydrator<Request, ProductCategory> ProductIntervalHydrator;
	typedef ConstrainedReferenceHydrator<Request, Product, NormalPrFunction> ProductHydrator;

	RequestHydratorChain(OperationMode& opMode, RandomStream& random, GeneratorConfig& config) :
		HydratorChain<Request>(opMode, random),
		_hydrateServer(random, &Request::server, config.webServers(), config.func<CustomDiscreteProbability>("Pr[request.server]")),
		_hydrateProductCategory(random, &Request::productCategory, config.func<CustomDiscreteProbability>("Pr[request.product_category]")),
		_hydrateProductInterval(&Request::productInterval, &Request::productCategory, config.func<CustomDiscreteProbability>("Pr[product.category]"), config.cardinality("product")),
		_hydrateProduct(random, &Request::product, &Request::productInterval, config.generatorPool().get<ProductGenerator>(), config.func<NormalPrFunction>("Pr[request.product]")),
		_logger(Logger::get("hydrator.request"))
	{
	}

	virtual ~RequestHydratorChain()
	{
	}

	/**
	 * Object hydrating function.
	 */
	void operator()(AutoPtr<Request> recordPtr) const
	{
		ensurePosition(recordPtr->genID());

		AutoPtr<Session> sessionPtr = recordPtr->session();
		AutoPtr<Request> prevPtr = recordPtr->prev();

//		ID sessionID = sessionPtr->genID();
//		if (sessionID >= 43198ULL && sessionID <= 43202ULL)
//		{
//			_logger.information("hydrating request for session #345852ULL, setting special server value");
//			recordPtr->server(toString(recordPtr->genID()));
//		}

		// hydrate server
		_random.next();
		_hydrateServer(recordPtr);

		// hydrate count of immediate predecessors of type BROWSE
		if (!prevPtr.isNull() && prevPtr->type() == RT_BROWSE)
		{
			recordPtr->browseCount(prevPtr->browseCount()+1);
		}
		else
		{
			recordPtr->browseCount(0);
		}

		// hydrate timestamp
		/*
		 * algebra expression:
		 *
		 * let h1 be $.timestamp($.session().sessionStart() + sample("session.intra_request_time", stream($).next())))
		 * let h2 be $.timestamp($.prev().timestamp() + sample("session.intra_request_time", stream($).next())))
		 * in
		 * if [$.prev().isNull()]
		 *   h1
		 * else
		 *   h2
		 *
		 */
		if (prevPtr.isNull())
		{
			recordPtr->timestamp(sessionPtr->sessionStart() + Timespan(_random(1, 30), 0));
		}
		else
		{
			recordPtr->timestamp(prevPtr->timestamp() + Timespan(_random(1, 30), 0));
		}

		/*
		 * algebra expression:
		 *
		 * let h1 = $.type(RT_BROWSE)
		 * let h2 = $.type(sample("request.type", $.position() - $.session().numberOfRequests() - 1))
		 * in
		 * if [$.session().type() == ST_BROWSE]
		 *   h1
		 * else
		 *   h2
		 *
		 */

		// pick a request type based on some complex criteria
		SessionType sessionType = sessionPtr->type();
		if (sessionType == ST_BROWSE)
		{
			recordPtr->type(RT_BROWSE);
		}
		else if (sessionType == ST_PURCHASE_ABORT || sessionType == ST_PURCHASE_SUBMIT)
		{
			// type of the first request in a purchase session is always BROWSE
			if (prevPtr.isNull())
			{
				recordPtr->type(RT_BROWSE);
			}
			// otherwise request type depends on the position
			else
			{
				I32u pos = recordPtr->position(); // position in the session
				I32u tot = sessionPtr->numberOfRequests(); // total number of requests in the session
				I32u rem = tot - pos - 1; // remaining requests in the session

				if (0 == rem)
				{
					if (sessionType == ST_PURCHASE_ABORT)
					{
						recordPtr->type(RT_PURCHASE_ABORT);
					}
					else
					{
						recordPtr->type(RT_PURCHASE_SUBMIT);
					}
				}
				else if (1 == rem)
				{
					recordPtr->type(RT_PURCHASE);
				}
				else // 2 or more requests remaining
				{
					I16 browseCount = recordPtr->browseCount();

					double p;

					if (browseCount < 1)
					{
						p = 0.0;
					}
					else if (browseCount < 2)
					{
						p = 0.1;
					}
					else if (browseCount < 3)
					{
						p = 0.2;
					}
					else if (browseCount < 4)
					{
						p = 0.4;
					}
					else if (browseCount < 5)
					{
						p = 0.8;
					}
					else
					{
						p = 1.0;
					}

					double x = _random.next();

					if (x > p)
					{
						recordPtr->type(RT_BROWSE);
					}
					else
					{
						recordPtr->type(RT_ADD_TO_CART);
					}
				}
			}
		}
		else
		{
			throw LogicException("Unknown session type");
		}

		/*
		 * algebra expression:
		 *
		 * let h1 = $.method(HTTP_METHOD_POST)
		 * let h2 = $.method(HTTP_METHOD_GET)
		 * in
		 * if [$.type() == RT_PURCHASE_ABORT || $.type() == RT_PURCHASE_SUBMIT]
		 *   h1
		 * else
		 *   h2
		 *
		 */
		RequestType requestType = recordPtr->type();
		if (requestType == RT_PURCHASE_ABORT || requestType == RT_PURCHASE_SUBMIT)
		{
			recordPtr->method(POST);
		}
		else
		{
			recordPtr->method(GET);
		}


		/*
		 * algebra expression:
		 *
		 * let h1 = ...
		 * let h2 = $.productInterval(...)
		 * let h3 = $.product(...)
		 * in
		 * if [$.type() == RT_BROWSE]
		 *   if [sample("request.same_product_category", stream($).next()) < 0.8]
		 *   else []
		 * else
		 *   h2
		 *
		 */
		if (requestType == RT_BROWSE)
		{
			double x = _random.next();

			if (x < 0.8)
			{
				recordPtr->productCategory(sessionPtr->primaryProductCategory());
			}
			else
			{
				_hydrateProductCategory(recordPtr);
			}

			_hydrateProductInterval(recordPtr);
			_hydrateProduct(recordPtr);
		}
		else if (requestType == RT_ADD_TO_CART)
		{
			recordPtr->productCategory(prevPtr->productCategory());
			recordPtr->product(prevPtr->product());
			recordPtr->browseCount(0);
		}
	}

protected:

	// hydrator instances
	ServerHydrator _hydrateServer;
	ProductCategoryHydrator _hydrateProductCategory;
	ProductIntervalHydrator _hydrateProductInterval;
	ProductHydrator _hydrateProduct;

	/**
	 * Logger instance.
	 */
	Logger& _logger;
};


// ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~
// custom generation task
// ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~

class SessionRequestsGeneratingTask : public StageTask<Request>
{
public:

	SessionRequestsGeneratingTask(RandomSetGenerator<Request>& generator, const GeneratorConfig& config) :
		StageTask<Request> (generator.name() + "::generate_records", generator.name(), config, false),
		_generator(generator),
		_random(generator.random()),
		_hydrate(generator.hydratorChain(BaseHydratorChain::SEQUENTIAL, _random)),
		_logger(Logger::get("task.session.requests"))
	{

		// adjust initial genID
		_maxItemsPerParent = _generator.config().getInt("partitioning." + _generator.name() + ".items-per-parent");
		_first = _generator.config().genIDBegin(_generator.name());
		_last = _generator.config().genIDEnd(_generator.name());
		_current = _generator.config().genIDBegin(_generator.name());
		// reset progress counter
		_progressCounter = 0;
		// adjust random position
		_random.atChunk(_current);
	}

	~SessionRequestsGeneratingTask()
	{
	}

	void onNewSession(const void* sender, AutoPtr<Session>& userSession)
	{
		if (userSession->numberOfRequests() > _maxItemsPerParent)
		{
			throw Myriad::ConfigException("Too much children requested for parent");
		}

		AutoPtr<Request> prevPtr;

		// generate e-tailer transactions
		for (I32u i = 0; i < userSession->numberOfRequests(); i++)
		{
			// create a new transaction
			AutoPtr<Request> requestPtr = _generator();
			requestPtr->genID(_current);
			requestPtr->position(i);
			requestPtr->session(userSession);
			requestPtr->prev(prevPtr);

			// apply hydrator chain
			_hydrate(requestPtr);

			prevPtr = requestPtr;

			// emit the transaction
			StageTask<Request>::_out.collect(*requestPtr);

			// advance the genID iterator variable
			_current++;
			// advance PRNG stream to the next chunk
			_random.nextChunk();

//			if(++_progressCounter >= 100000)
//			{
//				_progressCounter = 0;
//				StageTask<Request>::_progress = (_current - _first) / static_cast<Decimal>(_last - _first);
//			}
		}

		// FIXME: this is a quick fix, use random access method instead
		for (I32u i = userSession->numberOfRequests(); i < _maxItemsPerParent; i++)
		{
			// advance the genID iterator variable
			_current++;
			// advance PRNG stream to the next chunk
			_random.nextChunk();

//			if(++_progressCounter >= 100000)
//			{
//				_progressCounter = 0;
//				StageTask<Request>::_progress = (_current - _first) / static_cast<Decimal>(_last - _first);
//			}
		}
	}

private:

	/**
	 * A reference to the generator creating the stage.
	 */
	RandomSetGenerator<Request>& _generator;

	/**
	 * A copy of the generator's random stream.
	 */
	RandomStream _random;

	/**
	 * A hydrator chain instance.
	 */
	RequestHydratorChain _hydrate;

	/**
	 * The max. number of items that can be generated per parent.
	 */
	ID _maxItemsPerParent;

	/**
	 * First genID in the current subsequence.
	 */
	ID _first;

	/**
	 * Last genID in the current subsequence.
	 */
	ID _last;

	/**
	 * Iterator variable that holds the current genID.
	 */
	ID _current;

	/**
	 * Progress counter.
	 */
	I16u _progressCounter;

	/**
	 * Logger instance.
	 */
	Logger& _logger;
};

// ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~
// base method definitions (don't modify)
// ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~

inline RequestHydratorChain RequestGenerator::hydratorChain(BaseHydratorChain::OperationMode opMode, RandomStream& random)
{
	return RequestHydratorChain(opMode, random, _config);
}

inline void RequestGenerator::prepare(Stage stage, const GeneratorPool& pool)
{
	// call generator implementation
	RandomSetGenerator<Request>::prepare(stage, pool);

	if (stage.name() == "default")
	{
		SessionRequestsGeneratingTask* executor = new SessionRequestsGeneratingTask(*this, _config);
		registerTask(executor);

		SessionGenerator& parentGenerator = pool.get<SessionGenerator>();
		parentGenerator.newRecord += Poco::priorityDelegate(executor, &SessionRequestsGeneratingTask::onNewSession, 1);
	}
}

} // namespace ClickStreamGen

#endif /* REQUESTGENERATOR_H_ */

