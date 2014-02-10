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

#ifndef PRODUCT_H_
#define PRODUCT_H_

#include "record/Record.h"

using namespace Myriad;

namespace ClickStreamGen
{

// forward declarations
class ProductGenerator;
class ProductHydratorChain;

enum ProductCategory
{
	PC_COMPUTER, PC_TV_SAT, PC_HIFI, PC_FOTO_VIDEO, PC_MOBILE, PC_MUSIC, PC_DVD_BLURAY, PC_GAMES_CONSOLES, PC_SOFTWARE, PC_SPECIAL_OFFERS
};

// Product class
class Product: public Record
{
public:

	Product()
	{
	}

	const I64u& key() const;
	void key(const I64u& v);

	const ProductCategory& category() const;
	void category(const ProductCategory& v);

	const Interval<ID>& categoryInterval() const;
	void categoryInterval(const Interval<ID>& v);

private:

	I64u _key;
	ProductCategory _category;
	Interval<ID> _category_interval;
};

inline const I64u& Product::key() const
{
	return _key;
}

inline void Product::key(const I64u& v)
{
	_key = v;
}

inline const ProductCategory& Product::category() const
{
	return _category;
}

inline void Product::category(const ProductCategory& v)
{
	_category = v;
}

inline const Interval<ID>& Product::categoryInterval() const
{
	return _category_interval;
}

inline void Product::categoryInterval(const Interval<ID>& v)
{
	_category_interval = v;
}

} // namespace ClickStreamGen

namespace Myriad
{

// record traits specialization
template<> struct RecordTraits<ClickStreamGen::Product>
{
	typedef ClickStreamGen::ProductGenerator GeneratorType;
	typedef ClickStreamGen::ProductHydratorChain HydratorChainType;
};

// forward declaration of operator<<
OutputCollector::StreamType& operator<<(OutputCollector::StreamType& out, const ClickStreamGen::Product& record);

// enum type conversion methods
inline std::string toString(const ClickStreamGen::ProductCategory& t)
{
	if (t == ClickStreamGen::PC_COMPUTER)
	{
		return "computer";
	}
	else if (t == ClickStreamGen::PC_TV_SAT)
	{
		return "tv-and-satelite";
	}
	else if (t == ClickStreamGen::PC_HIFI)
	{
		return "hifi";
	}
	else if (t == ClickStreamGen::PC_FOTO_VIDEO)
	{
		return "foto_video";
	}
	else if (t == ClickStreamGen::PC_MOBILE)
	{
		return "mobile";
	}
	else if (t == ClickStreamGen::PC_MUSIC)
	{
		return "music";
	}
	else if (t == ClickStreamGen::PC_DVD_BLURAY)
	{
		return "dvd-and-bluray";
	}
	else if (t == ClickStreamGen::PC_GAMES_CONSOLES)
	{
		return "games-and-consoles";
	}
	else if (t == ClickStreamGen::PC_SOFTWARE)
	{
		return "software";
	}
	else if (t == ClickStreamGen::PC_SPECIAL_OFFERS)
	{
		return "special-offers";
	}

	return "<unknown ProductCategory>";
}

template<> inline ClickStreamGen::ProductCategory toEnum<ClickStreamGen::ProductCategory>(int v)
{
	if (v == 0)
	{
		return ClickStreamGen::PC_COMPUTER;
	}
	else if (v == 1)
	{
		return ClickStreamGen::PC_TV_SAT;
	}
	else if (v == 2)
	{
		return ClickStreamGen::PC_HIFI;
	}
	else if (v == 3)
	{
		return ClickStreamGen::PC_FOTO_VIDEO;
	}
	else if (v == 4)
	{
		return ClickStreamGen::PC_MOBILE;
	}
	else if (v == 5)
	{
		return ClickStreamGen::PC_MUSIC;
	}
	else if (v == 6)
	{
		return ClickStreamGen::PC_DVD_BLURAY;
	}
	else if (v == 7)
	{
		return ClickStreamGen::PC_GAMES_CONSOLES;
	}
	else if (v == 8)
	{
		return ClickStreamGen::PC_SOFTWARE;
	}
	else if (v == 9)
	{
		return ClickStreamGen::PC_SPECIAL_OFFERS;
	}
}

} // namespace Myriad

#endif /* PRODUCT_H_ */
