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

#ifndef SESSIONGENERATOR_H_
#define SESSIONGENERATOR_H_

#include "generator/RandomSetGenerator.h"
#include "generator/UserGenerator.h"
#include "hydrator/EnumSetHydrator.h"
#include "record/Session.h"

using namespace Myriad;
using namespace Poco;

namespace ClickStreamGen {

class SessionHydratorChain;

class SessionGenerator: public RandomSetGenerator<Session>
{
public:

	typedef RecordTraits<Session>::HydratorChainType HydratorChainType;

	SessionGenerator(const string& name, GeneratorConfig& config, NotificationCenter& notificationCenter) :
		RandomSetGenerator<Session>(name, config, notificationCenter)
	{
	}

	void prepare(Stage stage, const GeneratorPool& pool)
	{
		// call generator implementation
		RandomSetGenerator<Session>::prepare(stage, pool);

		if (stage.name() == "default")
		{
			registerTask(new RandomSetTimeSpanGeneratingTask<Session, NormalPrFunction> (*this, _config, &Session::sessionStart, true));
		}
	}

	HydratorChainType hydratorChain(BaseHydratorChain::OperationMode opMode, RandomStream& random);
};

/**
 * Hydrator specialization for Session.
 */
class SessionHydratorChain : public HydratorChain<Session>
{
public:

	// hydrator type aliases
	typedef EnumSetHydrator<Session, SessionType> SessionTypeHydrator;
//	typedef EnumSetHydrator<Session, I32u> NumberOfRequestsHydrator;
	typedef EnumSetHydrator<Session, ProductCategory> ProductCategoryHydrator;

	SessionHydratorChain(OperationMode& opMode, RandomStream& random, GeneratorConfig& config) :
		HydratorChain<Session>(opMode, random),
		_hydratoeSessionType(random, &Session::type, config.func<CustomDiscreteProbability>("Pr[session.type]")),
//		_hydrateNumberOfRequests(random, &Session::numberOfRequests, config.getInt("partitioning.request.items-per-parent")),
		_hydratePrimaryProductCategory(random, &Session::primaryProductCategory, config.func<CustomDiscreteProbability>("Pr[session.primary_category]")),
		_numberOfRequestsPr(config.func<NormalPrFunction>("Pr[session.number_of_requests]")),
		_userInspector(config.generatorPool().get<UserGenerator>().inspector()),
		_maxNumberOfRequests(config.getInt("partitioning.request.items-per-parent")),
		_userCardinality(config.cardinality("user")),
		_ipAddressMin(3221225473U),
		_ipAddressMax(3221225473U + config.cardinality("user")),
		_config(config),
		_logger(Logger::get("hydrator.session"))
	{
	}

	virtual ~SessionHydratorChain()
	{
	}

	/**
	 * Object hydrating function.
	 */
	void operator()(AutoPtr<Session> recordPtr) const
	{
		ensurePosition(recordPtr->genID());

		// apply hydrators
		_hydratoeSessionType(recordPtr);

		// set number of requests based on type
		I32 numberOfRequests = static_cast<I32>(0.5 + _numberOfRequestsPr.sample(_random()));
		if (recordPtr->type() == ST_BROWSE)
		{
			if (numberOfRequests < 1)
			{
				numberOfRequests = 1;
			}
			else if (numberOfRequests > _maxNumberOfRequests)
			{
				numberOfRequests = _maxNumberOfRequests;
			}

		}
		else if (recordPtr->type() == ST_PURCHASE_SUBMIT || recordPtr->type() == ST_PURCHASE_ABORT)
		{
			if (numberOfRequests < 4)
			{
				numberOfRequests = 4;
			}
			else if (numberOfRequests > _maxNumberOfRequests)
			{
				numberOfRequests = _maxNumberOfRequests;
			}
		}
		recordPtr->numberOfRequests(numberOfRequests);

//		_hydrateNumberOfRequests(recordPtr);
		_hydratePrimaryProductCategory(recordPtr);

		if (recordPtr->genID() == 43200ULL)
		{
			_logger.information("hydrating session #345852ULL, setting special sessionStart");
			recordPtr->sessionStart(DateTime(2012, 3, 3, 12, 00, 00));
		}

		AutoPtr<User> user = new User();
		user->ipAddress(_random(_ipAddressMin, _ipAddressMax)); // sample a class C ip address
		recordPtr->user(user);
//		recordPtr->user(const_cast<RandomSetInspector<User>& >(_userInspector).at(_random(0, _userCardinality)));
	}

protected:


	// hydrator instances
	SessionTypeHydrator _hydratoeSessionType;
//	NumberOfRequestsHydrator _hydrateNumberOfRequests;
	ProductCategoryHydrator _hydratePrimaryProductCategory;

	NormalPrFunction& _numberOfRequestsPr;

	// inspector instances
	RandomSetInspector<User> _userInspector;

	// extra parameters used by the hydrator
	I32 _maxNumberOfRequests;
	I32u _userCardinality;

	I32u _ipAddressMin;
	I32u _ipAddressMax;

	/**
	 * GeneratorConfig instance.
	 */
	GeneratorConfig& _config;

	/**
	 * Logger instance.
	 */
	Logger& _logger;
};

// ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~
// base method definitions (don't modify)
// ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~

inline SessionHydratorChain SessionGenerator::hydratorChain(BaseHydratorChain::OperationMode opMode, RandomStream& random)
{
	return SessionHydratorChain(opMode, random, _config);
}

} // namespace ClickStreamGen

#endif /* SESSIONGENERATOR_H_ */

