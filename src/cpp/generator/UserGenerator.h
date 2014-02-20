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

#ifndef USERGENERATOR_H_
#define USERGENERATOR_H_

#include "generator/RandomSetGenerator.h"
#include "hydrator/MultiplicativeGroupHydrator.h"
#include "record/User.h"

using namespace Myriad;
using namespace Poco;

namespace ClickStreamGen {

class UserHydratorChain;

class UserGenerator: public RandomSetGenerator<User>
{
public:

	typedef RecordTraits<User>::HydratorChainType HydratorChainType;

	UserGenerator(const string& name, GeneratorConfig& config, NotificationCenter& notificationCenter) :
		RandomSetGenerator<User>(name, config, notificationCenter)
	{
	}

	void prepare(Stage stage, const GeneratorPool& pool)
	{
		// call generator implementation
		RandomSetGenerator<User>::prepare(stage, pool);

        if (stage.name() == "default")
        {
            registerTask(new RandomSetDefaultGeneratingTask<User>(*this, _config, false));
        }
	}

	HydratorChainType hydratorChain(BaseHydratorChain::OperationMode opMode, RandomStream& random);
};

/**
 * Hydrator specialization for User.
 */
class UserHydratorChain : public HydratorChain<User>
{
public:

	// hydrator type aliases
	typedef MultiplicativeGroupHydrator<User> KeyHydrator;

	UserHydratorChain(OperationMode& opMode, RandomStream& random, GeneratorConfig& config) :
		HydratorChain<User>(opMode, random),
		_hydrateKey(&User::key, config.cardinality("user"), config.genIDBegin("user"), config.genIDEnd("user"), opMode == RANDOM),
		_logger(Logger::get("hydrator.user"))
	{
	}

	virtual ~UserHydratorChain()
	{
	}

	/**
	 * Object hydrating function.
	 */
	void operator()(AutoPtr<User> recordPtr) const
	{
		ensurePosition(recordPtr->genID());

		// apply hydrators
		recordPtr->key(recordPtr->genID()+1);
		recordPtr->ipAddress(_random(3221225473U, 3758096383U)); // sample a class C ip address
	}

protected:

	// hydrator instances
	KeyHydrator _hydrateKey;

	/**
	 * Logger instance.
	 */
	Logger& _logger;
};

// ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~
// base method definitions (don't modify)
// ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~

inline UserHydratorChain UserGenerator::hydratorChain(BaseHydratorChain::OperationMode opMode, RandomStream& random)
{
	return UserHydratorChain(opMode, random, _config);
}

} // namespace ClickStreamGen

#endif /* USERGENERATOR_H_ */

