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

#ifndef GENERATORCONFIG_H_
#define GENERATORCONFIG_H_

#include "config/AbstractGeneratorConfig.h"

namespace Myriad {

class GeneratorConfig: public AbstractGeneratorConfig
{
public:

	GeneratorConfig(GeneratorPool& generatorPool) : AbstractGeneratorConfig(generatorPool)
	{
	}

	// define const reference getters for each member
	const vector<String>& webServers()
	{
		return web_servers;
	}

protected:

	void configureSets(const AutoPtr<XML::Document>& doc)
	{
		// bind string sets to config members with the bindStringSet method
		bindStringSet(doc, "web_servers", web_servers);
		// bind object sets to config members with the bindObjectSet method
	}

private:

	// define private members for each bound set
	vector<String> web_servers;
};

} // namespace Myriad

#endif /* GENERATORCONFIG_H_ */
