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

#ifndef PRODUCTGENERATOR_H_
#define PRODUCTGENERATOR_H_

#include "generator/RandomSetGenerator.h"
#include "hydrator/ClusteredEnumSetHydrator.h"
#include "hydrator/MultiplicativeGroupHydrator.h"
#include "record/Product.h"

using namespace Myriad;
using namespace Poco;

namespace ClickStreamGen {

class ProductHydratorChain;

class ProductGenerator: public RandomSetGenerator<Product>
{
public:

	typedef RecordTraits<Product>::HydratorChainType HydratorChainType;

	ProductGenerator(const string& name, GeneratorConfig& config, NotificationCenter& notificationCenter) :
		RandomSetGenerator<Product>(name, config, notificationCenter)
	{
	}

	void prepare(Stage stage, const GeneratorPool& pool)
	{
		// call generator implementation
		RandomSetGenerator<Product>::prepare(stage, pool);
	}

	HydratorChainType hydratorChain(BaseHydratorChain::OperationMode opMode, RandomStream& random);
};

/**
 * Hydrator specialization for Product.
 */
class ProductHydratorChain : public HydratorChain<Product>
{
public:

	// hydrator type aliases
	typedef MultiplicativeGroupHydrator<Product> KeyHydrator;
	typedef ClusteredEnumSetHydrator<Product, ProductCategory> ProductCategoryHydrator;

	ProductHydratorChain(OperationMode& opMode, RandomStream& random, GeneratorConfig& config) :
		HydratorChain<Product>(opMode, random),
		_hydrateKey(&Product::key, config.cardinality("product"), config.genIDBegin("product"), config.genIDEnd("product"), opMode == RANDOM),
		_hydrateProductCategory(&Product::category, &Product::categoryInterval, config.func<CustomDiscreteProbability>("Pr[product.category]"), config.cardinality("product")),
		_logger(Logger::get("hydrator.product"))
	{
	}

	virtual ~ProductHydratorChain()
	{
	}

	/**
	 * Object hydrating function.
	 */
	void operator()(AutoPtr<Product> recordPtr) const
	{
		ensurePosition(recordPtr->genID());

		// apply hydrators
		_hydrateKey(recordPtr);
		_hydrateProductCategory(recordPtr);
	}

protected:

	// hydrator instances
	KeyHydrator _hydrateKey;
	ProductCategoryHydrator _hydrateProductCategory;

	/**
	 * Logger instance.
	 */
	Logger& _logger;
};

// ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~
// base method definitions (don't modify)
// ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~

inline ProductHydratorChain ProductGenerator::hydratorChain(BaseHydratorChain::OperationMode opMode, RandomStream& random)
{
	return ProductHydratorChain(opMode, random, _config);
}

} // namespace ClickStreamGen

#endif /* PRODUCTGENERATOR_H_ */

