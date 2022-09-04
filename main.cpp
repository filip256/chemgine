#include <iostream>
#include <string>
#include <vector>
#include <array>
#include <map>
#include <fstream>
#include <algorithm>

#define IGNORE_CHECKSUM true
#define CHECKSUM_SEED 2957289472
#define MOLAR_EXISTANCE_THRESHOLD 0.0000001 // molar amounts smaller that this (0.1 umol) can be ignored

template <class T = char> class StatusCode
{
public:
	int code;
	std::string message;
	T value;

	StatusCode(const int statusCode) :
		code(statusCode),
		message("--Message not provided--"),
		value(NULL)
	{}

	StatusCode(const int statusCode, const std::string& statusMessage) :
		code(statusCode),
		message(statusMessage),
		value(NULL)
	{}

	StatusCode(const int statusCode, const std::string& statusMessage, const T& statusValue) :
		code(statusCode),
		message(statusMessage),
		value(statusValue)
	{}

	StatusCode(const T& statusValue) :
		code(200),
		message("Ok"),
		value(statusValue)
	{}

	StatusCode(const StatusCode<>& statusCode) :
		code(statusCode.code),
		message(statusCode.message)
		//value(NULL)
	{}

	inline bool operator== (StatusCode<> x) { return code == x.code; }

	static const StatusCode<> ConversionError; //35

	static const StatusCode<> FileCouldNotOpen; //41
	static const StatusCode<> FileNotOpen; //42
	static const StatusCode<> FileEmpty; //43
	static const StatusCode<> FileCorrupted; //45
	static const StatusCode<> BadFormat; //47

	static const StatusCode<> Ok; //200
	static const StatusCode<> Bad; //200

	static const StatusCode<> True; //201
	static const StatusCode<> False; //401

	static const StatusCode<> NotFound; //404
};

const StatusCode<> StatusCode<>::ConversionError(35, "Conversion error");
const StatusCode<> StatusCode<>::FileCouldNotOpen(41, "File could not have been opened");
const StatusCode<> StatusCode<>::FileNotOpen(42, "File is not open");
const StatusCode<> StatusCode<>::FileEmpty(43, "File is empty");
const StatusCode<> StatusCode<>::FileCorrupted(45, "File failed checksum verification");
const StatusCode<> StatusCode<>::BadFormat(47, "Error encountered while processing data");

const StatusCode<> StatusCode<>::Ok(200, "Ok");
const StatusCode<> StatusCode<>::True(201, "True");
const StatusCode<> StatusCode<>::Bad(400, "Bad");
const StatusCode<> StatusCode<>::False(401, "False");
const StatusCode<> StatusCode<>::NotFound(404, "Object not found");


namespace tools
{
	//parse csv type line
	std::vector<std::string> parseList(const std::string& csvLine, const char sep)
	{
		std::vector<std::string> outVector;
		
		const std::string::size_type size = csvLine.size();
		std::string::size_type lastComma = -1;

		for(std::string::size_type i = 0; i < size; ++i) 
			if (csvLine[i] == sep)
			{
				outVector.push_back(csvLine.substr(lastComma + 1, i - lastComma - 1));
				lastComma = i;
			}

		outVector.push_back(csvLine.substr(lastComma + 1, std::string::npos));
		return outVector;
	}

	bool stringToBool(const std::string& string)
	{
		if (string == "yes" || string == "true" || string == "1")
			return true;
		return false;
	}

	bool checkAll(const std::string& string, bool(*condition)(const char c))
	{
		const std::string::size_type size = string.size();
		for (std::string::size_type i = 0; i < size; ++i)
			if (condition(string[i]))
				return true;
		return false;
	}
	bool checkAtLeastTwo(const std::string& string, bool(*condition)(const char c)) // checks if a condition is fullfiled at leaft twice
	{
		bool first = false;
		const std::string::size_type size = string.size();
		for (std::string::size_type i = 0; i < size; ++i)
			if (condition(string[i]))
			{
				if (first)
					return true;
				else
					first = true;
			}
		return false;
	}

	constexpr long long unsigned int gcd(unsigned int a, unsigned int b)
	{
		//Stein algorithm
		if (a == 0)
			return b;
		if (b == 0)
			return a;

		int k = 0;
		while (((a | b) & 1) == 0)
		{
			a >>= 1;
			b >>= 1;
			++k;
		}

		while ((a & 1) == 0)
			a >>= 1;

		do
		{
			while ((b & 1) == 0)
				b >>= 1;

			if (a > b)
				std::swap(a, b);

			b -= a;
		} while (b != 0);

		long long unsigned int ret = a; //avoid overflow
		return ret << k;
	}

	constexpr long long unsigned int lcm(unsigned int a, unsigned int b)
	{
		return a / gcd(a, b) * b;
	}
}

namespace files 
{
	size_t createChecksum(const std::string& file)
	{
		std::ifstream stream(file);

		if (!stream.is_open())
			return 0;

		size_t computeChecksum = CHECKSUM_SEED;
		std::hash<std::string> stringHash;
		std::string buffer;
		while (std::getline(stream, buffer))
		{
			//TODO: use a faster, non-crypto hash algh
			computeChecksum ^= stringHash(buffer);
		}

		return computeChecksum;
	}

	//if successful, set stream to first line after checksum
	//not responsible for closing the stream
	const StatusCode<> verifyChecksum(std::ifstream& stream)
	{
		if (!stream.is_open())
			return StatusCode<>::FileNotOpen;

		std::string buffer;
		std::getline(stream, buffer);

		if (buffer.size() == 0)
			return StatusCode<>::FileEmpty;
		std::streampos afterFirstLine = stream.tellg();

		// TODO: remove this on production
		if(IGNORE_CHECKSUM)
			return StatusCode<>::Ok;

		//get checksum
		unsigned long long int checksum = strtoull(buffer.c_str(), nullptr, 10);
		if (checksum == 0ULL)
		{
			stream.seekg(0, stream.beg); //return stream to begin
			return StatusCode<>::ConversionError;
		}


		//compute checksum
		size_t computeChecksum = CHECKSUM_SEED;
		std::hash<std::string> stringHash;
		while (std::getline(stream, buffer))
		{
			//TODO: use a faster, non-crypto hash algh
			computeChecksum ^= stringHash(buffer);
		}

		//clear eofbit in order to use the stream further outside the function
		stream.clear();
	
		//verify
		if (checksum == computeChecksum)
		{
			stream.seekg(afterFirstLine, stream.beg); //return stream after first line
			return StatusCode<>::Ok;
		}

		stream.seekg(0, stream.beg); //return stream to begin
		return StatusCode<>::FileCorrupted;
	}
}

namespace util
{
	enum MeasureUnits
	{
		Unit = 0,
		Gram = 1,
		Liter = 2,
		Mole = 3
	};

	class Quantity
	{
		//store in standard measure unit (i.e. grams, moles)
		long double _standardAmount;
		MeasureUnits _unit;

	public:
		Quantity(const MeasureUnits unit) :
			_unit(unit),
			_standardAmount(0.0)
		{}

		Quantity(const long double amount) :
			_unit(Unit),
			_standardAmount(amount)
		{}

		Quantity(const MeasureUnits unit, const long double amount) :
			_unit(unit),
			_standardAmount(amount)
		{}

		MeasureUnits unit() const { return _unit; }
		long double asKilo() const { return _standardAmount / 1000.0; }
		long double asStd() const { return _standardAmount; }
		long double asMilli() const { return _standardAmount * 1000.0; }
		long double asMicro() const { return _standardAmount * 1000000.0; }
		std::string asString() const
		{
			std::string unitStr;
			switch (_unit) //TODO: change this if needed
			{
			case Gram:
				unitStr = "grams";
				break;
			case Liter:
				unitStr = "liters";
				break;
			case Mole:
				unitStr = "moles";
				break;
			}
			return std::to_string(_standardAmount) + " " + unitStr;
		}

		void setAsKilo(const long double amount) { _standardAmount = amount * 1000.0; }
		void set(const long double amount) { _standardAmount = amount; }
		void setAsMilli(const long double amount) { _standardAmount = amount / 1000.0; }
		void setAsMicro(const long double amount) { _standardAmount = amount / 1000000.0; }

		void addAsKilo(const long double amount) { _standardAmount += amount * 1000.0; }
		void add(const long double amount) { _standardAmount += amount; }
		void addAsMilli(const long double amount) { _standardAmount += amount / 1000.0; }
		void addAsMicro(const long double amount) { _standardAmount += amount / 1000000.0; }

		//modifies and returns
		Quantity& convertTo(const MeasureUnits unit, const long double conversionRate)
		{
			_unit = unit;
			_standardAmount *= conversionRate;
			return *this;
		}
		//just returns
		inline Quantity getConvertedTo(const MeasureUnits unit, const long double conversionRate) const 
		{
			return Quantity(unit, _standardAmount * conversionRate);
		}

		inline void operator+= (const long double x) { _standardAmount += x; }
		inline void operator-= (const long double x) { _standardAmount -= x; }
		inline void operator*= (const long double x) { _standardAmount *= x; }
		inline void operator/= (const long double x) { _standardAmount /= x; }
		inline void operator+= (const Quantity& x) { _standardAmount += x.asStd(); }
	};

	template <class T> class WithQuantity
	{
	public:
		Quantity amount;
		T item;

		WithQuantity(const T& object, const Quantity& quantity) :
			amount(quantity),
			item(object)
		{}

		//allows simpler synthax
		WithQuantity(const T& object) :
			amount(Unit, 1.0),
			item(object)
		{}


	};
}

namespace data
{
	class IonData
	{
	public:
		std::string _atomName, _ionName;
		std::vector<int> _charges;
		long double _atomicMass;
		bool _isPolyatomic;

	public:

		IonData(const std::string& atomName, const std::string& ionName, const std::vector<int>& charges, const long double atomicMass, const bool isPolyatomic) :
			_atomName(atomName),
			_ionName(ionName),
			_charges(charges),
			_atomicMass(atomicMass),
			_isPolyatomic(isPolyatomic)
		{}
	};

	class SubstanceData
	{
	public:
		std::string _name, _class;
		util::WithQuantity<std::string> _cationId, _anionId;
		long double _molecularMass, _density, _acidity, _meltingPoint, _boilingPoint, _solubility;
		int _reactivity;
		//TODO: sf::Color _color;

		SubstanceData(
			const util::WithQuantity<std::string> cationId,
			const util::WithQuantity<std::string> anionId,
			const std::string& name,
			const long double molecularMass,
			const long double density,
			const std::string& classType,
			const long double acidity,
			const int reactivity,
			const long double meltingPoint,
			const long double boilingPoint,
			const long double solubility
		) :
			_name(name),
			_class(classType),
			_cationId(cationId),
			_anionId(anionId),
			_molecularMass(molecularMass),
			_density(density),
			_acidity(acidity),
			_meltingPoint(meltingPoint),
			_boilingPoint(boilingPoint),
			_solubility(solubility),
			_reactivity(reactivity)
		{}


	};

	//base for data tables
	template<class T> class DataTable
	{
	protected:
		std::map<std::string, T> _table;
	public:
		DataTable() : _table()
		{

		}

		const std::map<std::string, T>& getData() const { return _table; }

		bool contains(const std::string id) const
		{
			return _table.find(id) != _table.end();
		}
	};

	class IonDataTable : public DataTable<IonData>
	{
	public:
		IonDataTable() : DataTable()
		{
			
		}

		IonDataTable(const IonDataTable&) = delete;

		const StatusCode<> loadFromFile(const std::string& dataFile)
		{
			std::ifstream file(dataFile);

			if (!file.is_open())
				return StatusCode<>::FileCouldNotOpen;

			if (files::verifyChecksum(file).code != 200) //not OK
				return StatusCode<>::FileCorrupted;

			//erase old table
			_table.erase(_table.begin(), _table.end());

			//parse file
			std::string buffer;
			while (std::getline(file, buffer))
			{
				//parse csv line
				auto line = tools::parseList(buffer, ',');

				//get vector of strings of charges
				auto stringCharges = tools::parseList(line[3], ' ');

				//convert strings to ints
				std::vector<int> charges;
				const std::vector<int>::size_type size = stringCharges.size();
				for (std::vector<int>::size_type i = 0; i < size; ++i)
					charges.push_back(std::stoi(stringCharges[i]));

				if (!_table.emplace(line[0], IonData(line[1], line[2], charges, std::stold(line[4]), tools::stringToBool(line[5]))).second)
					return StatusCode<>::BadFormat;
			}
			file.close();

			return StatusCode<>::Ok;
		}

		//getters
		inline const IonData& get(const std::string& id) const { return _table.at(id); }

		inline const std::string& name(const std::string& id) const { return _table.at(id)._ionName; }
		inline const std::string& atomName(const std::string& id) const { return _table.at(id)._atomName; }
		inline const std::vector<int>& charges(const std::string& id) const { return _table.at(id)._charges; }
		inline const long double mass(const std::string& id) const { return _table.at(id)._atomicMass; }
		inline const bool isPolyatomic(const std::string& id) const { return _table.at(id)._isPolyatomic; }
	};

	class SubstanceDataTable : public DataTable<SubstanceData>
	{
	public:
		SubstanceDataTable() : DataTable()
		{

		}

		SubstanceDataTable(const SubstanceDataTable&) = delete;

		const StatusCode<> loadFromFile(const std::string& dataFile)
		{
			std::ifstream file(dataFile);

			if (!file.is_open())
				return StatusCode<>::FileCouldNotOpen;

			if (files::verifyChecksum(file).code != 200) //not OK
				return StatusCode<>::FileCorrupted;

			//erase old table
			_table.erase(_table.begin(), _table.end());

			//parse file
			std::string buffer;
			while (std::getline(file, buffer))
			{
				//parse csv line
				auto line = tools::parseList(buffer, ',');

				//get cation and anion
				auto cationString = tools::parseList(line[1], ' ');
				auto anionString = tools::parseList(line[2], ' ');

				//add element
				if (!_table.emplace(line[0], SubstanceData(
					util::WithQuantity<std::string>(cationString[0], cationString.size() > 1 ? util::Quantity(std::stoi(cationString[1])) : 1), // unreadable ik
					util::WithQuantity<std::string>(anionString[0], anionString.size() > 1 ? util::Quantity(std::stoi(anionString[1])) : 1),    // if no value is specified in the table, use 1 as default
					line[3], std::stold(line[4]), std::stold(line[5]), line[6], std::stold(line[7]), std::stoi(line[8]), std::stold(line[9]),
					std::stold(line[10]), std::stold(line[11]))
				).second)
					return StatusCode<>::BadFormat;

			}
			file.close();

			return StatusCode<>::Ok;
		}

		//getters
		inline const SubstanceData& get(const std::string& id) const { return _table.at(id); }
		inline const util::WithQuantity<std::string> cation(const std::string& id) const { return _table.at(id)._cationId; }
		inline const util::WithQuantity<std::string> anion(const std::string& id) const { return _table.at(id)._anionId; }
		inline const int cationCount(const std::string& id) const { return _table.at(id)._cationId.amount.asStd(); }
		inline const int anionCount(const std::string& id) const { return _table.at(id)._anionId.amount.asStd(); }
		inline const std::string& name(const std::string& id) const { return _table.at(id)._name;  }
		inline const long double mass(const std::string& id) const { return _table.at(id)._molecularMass; }
		inline const long double density(const std::string& id) const { return _table.at(id)._density; }
		inline const std::string& type(const std::string& id) const { return _table.at(id)._class; }
		inline const long double acidity(const std::string& id) const { return _table.at(id)._acidity; }
		inline const int reactivity (const std::string& id) const { return _table.at(id)._reactivity; }
		inline const long double meltingPoint(const std::string& id) const { return _table.at(id)._meltingPoint; }
		inline const long double boilingPoint(const std::string& id) const { return _table.at(id)._boilingPoint; }
		inline const long double solubility(const std::string& id) const { return _table.at(id)._solubility; }

	};
}

namespace chem
{
	inline long double getAdjustedTemperature(const long double temperature, const long double pressure)
	{
		//Clausius–Clapeyron
		return 1.0 / ((8.31446261815324 / 2264.705) * log(760.0 / pressure) + 1.0 / (temperature + 273.15)) - 273.15;

		//
		//return temperature - 0.045 * (760.0 - pressure);
		//return temperature - 0.0965 * (760.0 - pressure); //30
		//return temperature - 0.042 * (760.0 - pressure); //500
		//return temperature - (0.16 - 1.1489361702127659574468085106383e-4 * (40 * pressure - 40)) * (760.0 - pressure);
		//1.1489361702127659574468085106383e-4

	}

	inline std::string getId(const std::string& cationId, const unsigned int cationCount, const std::string& anionId, const unsigned int anionCount)
	{
		// TODO : could be done more efficiently
		std::string id;
		if (cationCount == 1)
			id += cationId;
		else if (tools::checkAtLeastTwo(cationId, [](const char c) -> bool { return (c >= 'A' && c <= 'Z'); }))
			id += '(' + cationId + ')' + std::to_string(cationCount);
		else
			id += cationId + std::to_string(cationCount);

		if (anionCount == 1)
			id += anionId;
		else if (tools::checkAtLeastTwo(anionId, [](const char c) -> bool { return (c >= 'A' && c <= 'Z'); }))
			id += '(' + anionId + ')' + std::to_string(anionCount);
		else
			id += anionId + std::to_string(anionCount);

		return id;
	}

	enum SubstanceType
	{
		Inorganic = 1,
		Organic = 2
	};

	class Ion
	{
		std::string _id;
		static const data::IonDataTable* _dataRef;

	public:
		//copies data from the datastore to internal member properties
		//TODO: check if id exists!
		Ion(const std::string& id) :
			_id(id)
		{}

		// return copies from now on
		inline std::string name() const { return _dataRef->name(_id); }
		inline long double mass() const { return _dataRef->mass(_id); }
		inline std::vector<int> charges() const { return _dataRef->charges(_id); }
		inline bool isPolyatomic() const { return _dataRef->isPolyatomic(_id); }
		inline std::string abreviation() const { return _id; }

		//returns the index in charges list or -1
		int hasCharge(const int charge) const
		{
			const std::vector<int>& charges = _dataRef->charges(_id);
			std::vector<int>::size_type size = charges.size();
			for (std::vector<int>::size_type i = 0; i < size; ++i)
				if (charges[i] == charge)
					return i;
			return -1;
		}

		static void initialize(const data::IonDataTable* dataRef)
		{
			_dataRef = dataRef;
		}

	};
	const data::IonDataTable* Ion::_dataRef = nullptr;

	class Substance
	{
	protected:
		//statically store the location of data tables
		//no longer needed to pass table references on constructors but a call to initialize is needed before using the class
		static const data::SubstanceDataTable* _substanceRef;
		static const data::IonDataTable* _ionRef;

		std::string _id;
		util::Quantity _amount; //only stored as moles
		bool _isWellFormed;
		SubstanceType _type;

	public:
		Substance(const std::string& id, SubstanceType type, const util::Quantity& amount = util::Quantity(util::Mole, 1.0), const bool isWellFormed = true) : _id(id), _type(type), _amount(amount), _isWellFormed(isWellFormed) {}

		inline std::string id() const { return _id; }
		inline util::Quantity amount() const { return _amount; }
		inline util::Quantity& accessAmount() { return _amount; }

		inline const std::string& name() const { return _substanceRef->name(_id); }
		inline const long double mass() const { return _substanceRef->mass(_id); }
		inline const long double density() const { return _substanceRef->density(_id); }
		inline const std::string& type() const { return _substanceRef->type(_id); }
		inline const long double acidity() const { return _substanceRef->acidity(_id); }
		inline const int reactivity() const { return _substanceRef->reactivity(_id); }
		inline const long double meltingPoint() const { return _substanceRef->meltingPoint(_id); }
		inline const long double boilingPoint() const { return _substanceRef->boilingPoint(_id); }
		inline const long double solubility() const { return _substanceRef->solubility(_id); }
		inline const bool isWellFormed() const { return _isWellFormed;  }

		util::Quantity getQuantityConvertedTo(const util::MeasureUnits unit) const
		{
			if (_amount.unit() == util::Mole)
			{
				if (unit == util::Gram)
					return _amount.getConvertedTo(util::Gram, mass());
				if (unit == util::Liter)
					return _amount.getConvertedTo(util::Gram, mass()).getConvertedTo(util::Liter, 0.001 / density() );
			}
			return util::Quantity(0.0);
		}

		//help count instances in order to avoid leaks
		void* operator new(const std::size_t count)
		{
			++instanceCount;
			return ::operator new(count);
		}
		void operator delete(void *ptr)
		{
			--instanceCount;
			return ::operator delete(ptr);
		}

		static int instanceCount;
	};
	int Substance::instanceCount = 0;

	class InorganicSubstance : public Substance
	{
		util::WithQuantity<std::string> _cationId, _anionId; // <- could keep only one id, but access will take twice as much
		int _cationCharge, _anionCharge;
	public:
		InorganicSubstance(const std::string& id, const util::Quantity& amount = util::Quantity(util::Mole, 1.0)) :
			_cationId(util::WithQuantity<std::string>(_substanceRef->cation(id).item, _substanceRef->cation(id).amount)),
			_anionId(util::WithQuantity<std::string>(_substanceRef->anion(id).item, _substanceRef->anion(id).amount)),
			Substance(id, Inorganic, amount)
		{
			const std::vector<int>& cCharges = _ionRef->charges(_cationId.item);
			const std::vector<int>& aCharges = _ionRef->charges(_anionId.item);
			const std::vector<int>::size_type size1 = cCharges.size();
			const std::vector<int>::size_type size2 = aCharges.size();
			bool ok = false;
			for (std::vector<int>::size_type i = 0; i < size1 && !ok; ++i)
				for (std::vector<int>::size_type j = 0; j < size2; ++j)
					if (_cationId.amount.asStd() * cCharges[i] == -1 * (_anionId.amount.asStd() * aCharges[j]))
					{
						_cationCharge = cCharges[i];
						_anionCharge = aCharges[j];
						ok = true;
						break;
					}
		}

		InorganicSubstance(const util::WithQuantity<std::string>& cationId, const util::WithQuantity<std::string>& anionId) :
			_cationId(cationId),
			_anionId(anionId),
			Substance(getId(cationId.item, cationId.amount.asStd(), anionId.item, anionId.amount.asStd()), Inorganic)
		{
			//reduce coeficients if possible
			long long unsigned int gcd = tools::gcd(_cationId.amount.asStd(), _anionId.amount.asStd());
			_cationId.amount /= gcd;
			_anionId.amount /= gcd;
			_amount = util::Quantity(util::Mole, gcd);

			//check all combinations until a balanced one is found and set the isWellFormed flag
			//needed because ions can have multiple charges
			const std::vector<int>& cCharges = _ionRef->charges(cationId.item);
			const std::vector<int>& aCharges = _ionRef->charges(anionId.item);
			const std::vector<int>::size_type size1 = cCharges.size();
			const std::vector<int>::size_type size2 = aCharges.size();
			bool ok = false;
			for (std::vector<int>::size_type i = 0; i < size1 && !ok; ++i)
				for (std::vector<int>::size_type j = 0; j < size2; ++j)
					if (_cationId.amount.asStd() * cCharges[i] == -1 * (_anionId.amount.asStd() * aCharges[j]))
					{
						_cationCharge = cCharges[i];
						_anionCharge = aCharges[j];
						ok = true;
						break;
					}

			if (!ok)
				_isWellFormed = false;
		}

		InorganicSubstance(const std::string& cationId, const int cationCharge, const std::string& anionId, const int anionCharge) : // tries to build from knwo charges, needed for reactions
			_cationCharge(cationCharge),
			_anionCharge(anionCharge),
			_cationId(cationId),
			_anionId(anionId),
			Substance("", Inorganic) //the id is not knwon yet
		{
			if (Ion(cationId).hasCharge(cationCharge) == -1 || Ion(anionId).hasCharge(anionCharge) == -1)
			{
				_isWellFormed = false;
				return;
			}

			long long unsigned int lcm = tools::lcm(abs(cationCharge), abs(anionCharge));
			_cationId.amount.set(lcm / abs(cationCharge));
			_anionId.amount.set(lcm / abs(anionCharge));
			_id = getId(_cationId.item, _cationId.amount.asStd(), _anionId.item, _anionId.amount.asStd());
		}

		inline const util::WithQuantity<std::string> cation() const { return _cationId; }
		inline const util::WithQuantity<std::string> anion() const { return _anionId; }
		inline const std::string cationId() const { return _cationId.item; }
		inline const std::string anionId() const { return _anionId.item; }
		inline const int currentCationCharge() const { return _cationCharge; }
		inline const int currentAnionCharge() const { return _anionCharge; }
		inline const unsigned int absCurrentCationCharge() const { return abs(_cationCharge); }
		inline const unsigned int absCurrentAnionCharge() const { return abs(_anionCharge); }
		inline const int cationCount() const { return _substanceRef->cationCount(_id); } // get number of positive ions in one molecule
		inline const int anionCount() const { return _substanceRef->anionCount(_id); }   // get number of negative ions in one molecule

		bool isWellFormed() const { return _isWellFormed; }

		void printName()
		{
			std::cout << _substanceRef->name(_id) <<' '<< _cationCharge<<' '<<_anionCharge<<'\n';
		}
		void printAbreviation()
		{
			std::cout <<_id;
		}

		static void initialize(const data::IonDataTable* ionRef, const data::SubstanceDataTable* substanceRef)
		{
			_ionRef = ionRef;
			_substanceRef = substanceRef;
		}
	};
	const data::SubstanceDataTable* InorganicSubstance::_substanceRef = nullptr;
	const data::IonDataTable* InorganicSubstance::_ionRef = nullptr;

	const std::array<int, 4> balanceBinaryReaction(const InorganicSubstance& reactant1, const InorganicSubstance& reactant2, std::pair<int, int> productIonCount)
	{
		//this creates a system, creates an equation like a*A = b*B = c*C = d*D, sets the reaction coeficient with the largest system coeficient to 1 and solves for the other coeficients
		//the second product is not even needed but it is important that the substance with the largest system coeficient is present
		double systemCoef[4];
		systemCoef[0] = reactant1.cationCount();
		unsigned short int imax = 0;

		systemCoef[2] = productIonCount.first; //cation
		if (systemCoef[2] > systemCoef[imax]) imax = 2;

		systemCoef[1] = systemCoef[2] / productIonCount.second; //anion
		if (systemCoef[1] > systemCoef[imax]) imax = 1;

		systemCoef[3] = systemCoef[1] / reactant2.cationCount();
		if (systemCoef[3] > systemCoef[imax]) imax = 3;

		std::array<int, 4> reactionCoef;
		reactionCoef[imax] = 1;
		for (unsigned short int i = 0; i < 4; ++i)
			if (i != imax)
				reactionCoef[i] = reactionCoef[imax] * systemCoef[imax] / systemCoef[i];

		return reactionCoef;
	}

	class OrganicSubstance : Substance
	{

	};

	class SystemState //physical state of a system
	{
	public:
		long double _temperature, _pressure; //in C and torr
		SystemState(const long double temperature, const long double pressure) :
			_temperature(temperature),
			_pressure(pressure)
		{}

		inline long double temperature() const { return _temperature; }
		inline long double pressure() const { return _pressure; }
		inline void setTemperature(const long double temperature) { _temperature = temperature; }
		inline void setPressure(const long double pressure) { _pressure = pressure; }

		static SystemState Atmosphere;
	};
	SystemState SystemState::Atmosphere = SystemState(25, 760);

	class Mixture
	{
		class BaseReaction
		{
		protected:
			Mixture& _mixture;
			int _priority;

		public:
			BaseReaction(Mixture& mixture, const int priority) :
				_mixture(mixture),
				_priority(priority)
			{}

			virtual bool react() = 0;
		};

		class InorganicReaction : public BaseReaction
		{
			InorganicSubstance &_reactant1, &_reactant2, *_product1, *_product2;
			// reaction result is not needed if reactions are declared as static functions that receive *this
			std::array<int, 4> _coeficients;
			bool _isFirstCall; // <- flag for the first time react is called
			         
			// We store 3 function pointers to predefined functions:
			/// - checks if reaction is possible, called every time
			/// - computes data about the reaction and does it, only called the first time
			/// - does the reaction using the already computed data assuming that the products are already created, called every time exept for first
			bool(*_checkReaction)(const Mixture& mixture, const InorganicSubstance&, const InorganicSubstance&);                          
			void(*_computeReaction)(InorganicReaction&, const InorganicSubstance&, const InorganicSubstance&);                            
			bool(*_applyReaction)(const InorganicReaction&, Mixture&, const InorganicSubstance&, const InorganicSubstance&);

		public:
			InorganicReaction(Mixture& mixture, InorganicSubstance &reactant1, InorganicSubstance &reactant2, const int priority,
				bool(*checkReaction)(const Mixture&, const InorganicSubstance&, const InorganicSubstance&),
				void(*computeReaction)(InorganicReaction&, const InorganicSubstance&, const InorganicSubstance&),
				bool(*applyReaction)(const InorganicReaction&, Mixture&, const InorganicSubstance&, const InorganicSubstance&)) :
				_reactant1(reactant1),
				_reactant2(reactant2),
				_product1(nullptr),
				_product2(nullptr),
				_isFirstCall(true),
				_checkReaction(checkReaction),
				_computeReaction(computeReaction),
				_applyReaction(applyReaction),
				BaseReaction(mixture, priority)
			{}


			bool react() final override
			{
				if (!_checkReaction(_mixture, _reactant1, _reactant2))
					return false;

				if (_isFirstCall)
				{
					_isFirstCall = false;
					_computeReaction(*this, _reactant1, _reactant2);
					_applyReaction(*this, _mixture, _reactant1, _reactant2);
				}
				else
					_applyReaction(*this, _mixture, _reactant1, _reactant2);
			}

			inline static bool ck_inorgAcidBase(const Mixture& mixture, const InorganicSubstance& subst1, const InorganicSubstance& subst2)
			{
				return (subst1.amount().asStd() >= MOLAR_EXISTANCE_THRESHOLD && subst2.amount().asStd() >= MOLAR_EXISTANCE_THRESHOLD);
			}
			inline static void cp_inorgAcidBase(InorganicReaction& owner, const InorganicSubstance& subst1, const InorganicSubstance& subst2)
			{
				// compute the ion coeficients of the products
				std::pair<int, int> p1IonCount;
				long long unsigned int lcm1 = tools::lcm(subst1.absCurrentCationCharge(), subst2.absCurrentAnionCharge());
				p1IonCount.first = lcm1 / subst1.absCurrentCationCharge();
				p1IonCount.second = lcm1 / subst2.absCurrentAnionCharge();

				std::pair<int, int> p2IonCount;
				long long unsigned int lcm2 = tools::lcm(subst2.absCurrentCationCharge(), subst1.absCurrentAnionCharge());
				p1IonCount.first = lcm2 / subst2.absCurrentCationCharge();
				p1IonCount.second = lcm2 / subst1.absCurrentAnionCharge();

				// compute and verify (recompute) the reaction coeficients
				if (p1IonCount.first + p1IonCount.second >= p1IonCount.first + p1IonCount.second) // <- approximate the substance with largest system coeficient used for the function (check definition) ~80%
				{
					owner._coeficients = balanceBinaryReaction(subst1, subst2, p1IonCount);
					if (owner._coeficients[0] * subst1.cationCount() != owner._coeficients[2] * p1IonCount.first || owner._coeficients[1] * subst2.cationCount() != owner._coeficients[3] * p2IonCount.first) // check correctness
					{
						owner._coeficients = balanceBinaryReaction(subst2, subst1, p2IonCount);
						std::swap(owner._coeficients[0], owner._coeficients[1]);
						std::swap(owner._coeficients[2], owner._coeficients[3]);
					}
				}
				else
				{
					owner._coeficients = balanceBinaryReaction(subst2, subst1, p2IonCount);
					if (owner._coeficients[1] * subst1.cationCount() != owner._coeficients[3] * p1IonCount.first || owner._coeficients[0] * subst2.cationCount() != owner._coeficients[2] * p2IonCount.first) // check correctness
						owner._coeficients = balanceBinaryReaction(subst1, subst2, p1IonCount);
					else
					{ // swap coef to match the original substance order
						std::swap(owner._coeficients[0], owner._coeficients[1]);
						std::swap(owner._coeficients[2], owner._coeficients[3]);
					}
				}

				std::cout << owner._coeficients[0] << ' ' << subst1.id() << " + " << owner._coeficients[1] << ' ' << subst2.id() << "  -->  "
					<< owner._coeficients[2] << ' ' << getId(subst1.cationId(), subst1.cationCount(), subst2.anionId(), subst2.anionCount()) << " + "
					<< owner._coeficients[3] << ' ' << getId(subst2.cationId(), subst2.cationCount(), subst1.anionId(), subst1.anionCount()) << '\n';
			}
			inline static bool ap_inorgAcidBase(const InorganicReaction& owner, Mixture& mixture, const InorganicSubstance& subst1, const InorganicSubstance& subst2)
			{
				return true;
			}
		};


		std::vector<Substance*> _solidLayer, _denseNonpolarLayer, _polarLayer, _nonpolarLayer, _gasLayer; // <- owning
		SystemState _state;
		util::Quantity _totalVolume;

		void add(Substance* newSubstance)
		{
			if (!newSubstance->isWellFormed()) //mixtures cannot contain invalid substances
				return;

			_totalVolume += newSubstance->getQuantityConvertedTo(util::Liter);

			auto result = find(newSubstance->id());
			if (result == StatusCode<>::Ok) // if already present just incease its amount
			{
				(*result.value)->accessAmount() += newSubstance->amount();
				delete newSubstance;
			}
			else
			{
				if (newSubstance->meltingPoint() > _state.temperature())
					_solidLayer.push_back(newSubstance);
				else if (newSubstance->boilingPoint() > _state.temperature())
					_polarLayer.push_back(newSubstance);
				else
					_gasLayer.push_back(newSubstance);
			}
		}

	public:
		Mixture(const SystemState& state = SystemState::Atmosphere) :
			_state(state),
			_totalVolume(util::Liter, 0.0)
		{}
		~Mixture()
		{
			std::vector<Substance*>::size_type size = _solidLayer.size();
			for (std::vector<Substance*>::size_type i = 0; i < size; ++i)
				delete _solidLayer[i];

			size = _denseNonpolarLayer.size();
			for (std::vector<Substance*>::size_type i = 0; i < size; ++i)
				delete _denseNonpolarLayer[i];

			size = _polarLayer.size();
			for (std::vector<Substance*>::size_type i = 0; i < size; ++i)
				delete _polarLayer[i];

			size = _nonpolarLayer.size();
			for (std::vector<Substance*>::size_type i = 0; i < size; ++i)
				delete _nonpolarLayer[i];

			size = _gasLayer.size();
			for (std::vector<Substance*>::size_type i = 0; i < size; ++i)
				delete _gasLayer[i];
		}

		inline long double temperature() const { return _state.temperature(); }
		inline long double pressure() const { return _state.pressure(); }
		inline util::Quantity totalVolume() const { return _totalVolume; }

		void add(const InorganicSubstance& newSubstance)
		{
			if (!newSubstance.isWellFormed()) //mixtures cannot contain invalid substances
				return;

			_totalVolume += newSubstance.getQuantityConvertedTo(util::Liter);

			auto result = find(newSubstance.id());
			if (result == StatusCode<>::Ok) // if already present just incease its amount
			{
				(*result.value)->accessAmount() += newSubstance.amount();
			}
			else
			{
				Substance* temp = new InorganicSubstance(newSubstance); // <- non owning
				if (newSubstance.meltingPoint() > _state.temperature())
					_solidLayer.push_back(temp);
				else if (newSubstance.boilingPoint() > _state.temperature())
					_polarLayer.push_back(temp);
				else
					_gasLayer.push_back(temp);
			}
		}

		//the given mixture is emptied after its contents are moved
		void add(Mixture& mixture)
		{
			//compute the new state
			//TODO: be scientific
			_state.setTemperature(
				(_state.temperature() * _totalVolume.asStd() + mixture.temperature() * mixture.totalVolume().asStd()) / (_totalVolume.asStd() + mixture.totalVolume().asStd()));
			_state.setPressure(
				(_state.pressure() * _totalVolume.asStd() + mixture.pressure() * mixture.totalVolume().asStd()) / (_totalVolume.asStd() + mixture.totalVolume().asStd()));

			checkStates();

			std::vector<Substance*>::size_type size = mixture._polarLayer.size();
			for (std::vector<Substance*>::size_type i = 0; i < size; ++i)
				add(mixture._polarLayer[i]);

			size = mixture._solidLayer.size();
			for (std::vector<Substance*>::size_type i = 0; i < size; ++i)
				add(mixture._solidLayer[i]);

			size = mixture._gasLayer.size();
			for (std::vector<Substance*>::size_type i = 0; i < size; ++i)
				add(mixture._gasLayer[i]);

			mixture.empty();
		}

		StatusCode<std::vector<Substance*>::const_iterator> find(const std::vector<Substance*>& vector, const std::string& id) const
		{
			for (auto it = vector.begin(); it != vector.end(); ++it)
				if ((*it)->id() == id)
					return it;
			return StatusCode<>::NotFound;
		}
		StatusCode<std::vector<Substance*>::const_iterator> find(const std::string& id) const
		{
			auto result = find(_polarLayer, id);
			if (result == StatusCode<>::Ok)
				return result;

			result = find(_nonpolarLayer, id);
			if (result == StatusCode<>::Ok)
				return result;

			result = find(_denseNonpolarLayer, id);
			if (result == StatusCode<>::Ok)
				return result;

			result = find(_solidLayer, id);
			if (result == StatusCode<>::Ok)
				return result;

			result = find(_gasLayer, id);
			if (result == StatusCode<>::Ok)
				return result;

			return StatusCode<>::NotFound;
		}

		StatusCode<std::vector<Substance*>::const_iterator> find(const std::vector<Substance*>& vector, bool(*condition)(const Substance&)) const
		{
			for (auto it = vector.begin(); it != vector.end(); ++it)
				if (condition(**it))
					return it;
			return StatusCode<>::NotFound;
		}

		std::vector<std::vector<Substance*>::const_iterator> findAll(const std::vector<Substance*>& vector, bool(*condition)(const Substance&)) const
		{
			//TODO: maybe reserve size
			std::vector<std::vector<Substance*>::const_iterator> returnVector;
			for (auto it = vector.begin(); it != vector.end(); ++it)
				if (condition(**it))
					returnVector.push_back(it);
			return returnVector;
			
		}

		void checkStates()
		{
			//solid
			std::vector<Substance*>::size_type size = _solidLayer.size();
			for (std::vector<Substance*>::size_type i = 0; i < size; ++i)
			{
				if (_solidLayer[i]->boilingPoint() < _state.temperature())
				{
					_gasLayer.push_back(_solidLayer[i]);
					_solidLayer.erase(_solidLayer.begin() + i);
					--i;
					--size;
				}
				else if (_solidLayer[i]->meltingPoint() < _state.temperature())
				{
					_polarLayer.push_back(_solidLayer[i]);
					_solidLayer.erase(_solidLayer.begin() + i);
					--i;
					--size;
				}
			}

			//polar
			size = _polarLayer.size();
			for (std::vector<Substance*>::size_type i = 0; i < size; ++i)
			{
				if (_polarLayer[i]->boilingPoint() < _state.temperature())
				{
					_gasLayer.emplace_back(_polarLayer[i]);
					_polarLayer.erase(_polarLayer.begin() + i);
					--i;
					--size;
				}
				else if (_polarLayer[i]->meltingPoint() > _state.temperature())
				{
					_solidLayer.emplace_back(_polarLayer[i]);
					_polarLayer.erase(_polarLayer.begin() + i);
					--i;
					--size;
				}
			}

			//gas
			size = _gasLayer.size();
			for (std::vector<Substance*>::size_type i = 0; i < size; ++i)
			{
				if (_gasLayer[i]->boilingPoint() > _state.temperature())
				{
					_polarLayer.emplace_back(_gasLayer[i]);
					_gasLayer.erase(_gasLayer.begin() + i);
					--i;
					--size;
				}
				else if (_gasLayer[i]->meltingPoint() > _state.temperature())
				{
					_solidLayer.emplace_back(_gasLayer[i]);
					_gasLayer.erase(_gasLayer.begin() + i);
					--i;
					--size;
				}
			}
		}

		void empty()
		{
			_solidLayer.clear();
			_denseNonpolarLayer.clear();
			_polarLayer.clear();
			_nonpolarLayer.clear();
			_gasLayer.clear();

			_totalVolume = 0.0;
			_state.setPressure(760);
			_state.setTemperature(25);
		}

		//std::vector<Substance*>& solidLayer() { return _solidLayer; }
		//std::vector<Substance*>& denseNonpolarLayer() { return _denseNonpolarLayer; }
		//std::vector<Substance*>& polarLayer() { return _polarLayer; }
		//std::vector<Substance*>& nonpolarLayer() { return _nonpolarLayer; }
		//std::vector<Substance*>& gasLayer() { return _gasLayer; }

		void react()
		{
			//TODO: maybe this can be done async
			std::vector<std::vector<Substance*>::const_iterator> polarAcids = findAll(_polarLayer, [](const Substance& subst) -> bool { return subst.acidity() < 7.0; });
			std::vector<std::vector<Substance*>::const_iterator> polarBases = findAll(_polarLayer, [](const Substance& subst) -> bool { return subst.acidity() > 7.0; });

			//sort so that the most acidic and the most basic are first     !!!maybe sort after!!!
			std::sort(polarAcids.begin(), polarAcids.end(), [](const std::vector<Substance*>::const_iterator& x, const std::vector<Substance*>::const_iterator& y) -> bool { return (*x)->acidity() < (*y)->acidity(); });
			std::sort(polarBases.begin(), polarBases.end(), [](const std::vector<Substance*>::const_iterator& x, const std::vector<Substance*>::const_iterator& y) -> bool { return (*x)->acidity() > (*y)->acidity(); });

			const std::vector <std::vector<Substance*>::const_iterator>::size_type polarAcidsSize = polarAcids.size();
			const std::vector <std::vector<Substance*>::const_iterator>::size_type polarBasesSize = polarBases.size();
			for (auto i = 0; i < polarAcidsSize; ++i)
				for (auto j = 0; j < polarBasesSize; ++j)
					std::cout << (*polarAcids[i])->id() << " + " << (*polarBases[j])->id()<<'\n';

			auto a = InorganicSubstance("H2SO4", util::Quantity(util::Mole, 2.0));
			InorganicReaction r(*this, a, a, 5, InorganicReaction::ck_inorgAcidBase, InorganicReaction::cp_inorgAcidBase, InorganicReaction::ap_inorgAcidBase);

			//define reactions (it, it) that modify mixtures
			//compute reation priorities and add them to a queue
		}

		void printConstituents()
		{
			std::cout << "Total: " << _totalVolume.asMilli() <<"ml   "<<temperature()<< '\n';
			std::cout << "Solids:\n";
			for (int i = 0; i < _solidLayer.size(); ++i)
				std::cout <<" - "<< _solidLayer[i]->id() << ' ' << _solidLayer[i]->amount().asStd() << " moles\n";

			std::cout << "Liquids:\n";
			for (int i = 0; i < _polarLayer.size(); ++i)
				std::cout << " - "<< _polarLayer[i]->id() << ' ' << _polarLayer[i]->amount().asStd() << " moles\n";

			std::cout << "Gases:\n";
			for (int i = 0; i < _gasLayer.size(); ++i)
				std::cout << " - "<< _gasLayer[i]->id() << ' ' << _gasLayer[i]->amount().asStd() << " moles\n";
		}

	};

	namespace react
	{
		bool inorgAcidBase(const Mixture& mixture, const std::vector<InorganicSubstance*>::const_iterator& subst1, const std::vector<InorganicSubstance*>::const_iterator& subst2)
		{
			// check if the reaction can be done
			// TODO: see how this is handled, maybe remove them, maybe set a flag etc...
			if ((*subst1)->amount().asStd() < MOLAR_EXISTANCE_THRESHOLD || (*subst2)->amount().asStd() < MOLAR_EXISTANCE_THRESHOLD)
				return false;

			// balancing
			const unsigned int lcm1 = tools::lcm((*subst1)->absCurrentCationCharge() * (*subst1)->cationCount(), (*subst2)->absCurrentAnionCharge() * (*subst2)->anionCount());
			const unsigned int lcm2 = tools::lcm((*subst2)->absCurrentCationCharge() * (*subst2)->cationCount(), (*subst1)->absCurrentAnionCharge() * (*subst1)->anionCount());

			const unsigned int requiredSubst1 = tools::lcm((lcm1 / (*subst1)->absCurrentCationCharge()) / (*subst1)->cationCount(), (lcm2 / (*subst1)->absCurrentAnionCharge()) / (*subst1)->anionCount());
			const unsigned int requiredSubst2 = tools::lcm((lcm2 / (*subst2)->absCurrentCationCharge()) / (*subst2)->cationCount(), (lcm1 / (*subst2)->absCurrentAnionCharge()) / (*subst2)->anionCount());

			const unsigned int productAmount1 = requiredSubst1 * (*subst1)->cationCount() / (*subst2)->absCurrentAnionCharge();
			const unsigned int productAmount2 = requiredSubst1 * (*subst1)->anionCount() / (*subst2)->absCurrentCationCharge();


			//create the products of the reaction
			Substance *newSubst1 = new InorganicSubstance(getId((*subst1)->cation().item, requiredSubst1 * (*subst1)->cationCount() / productAmount1,
				(*subst2)->anion().item, requiredSubst2 * (*subst2)->anionCount() / productAmount1), util::Quantity(util::Mole, productAmount1));
			if (!newSubst1->isWellFormed())
			{
				delete newSubst1;
				return false;
			}
			/*Substance *newSubst2 = new InorganicSubstance((*subst2)->cation().item, (*subst2)->currentCationCharge(), (*subst1)->anion().item, (*subst1)->currentAnionCharge());
			if (!newSubst2->isWellFormed())
			{
				delete newSubst1;
				delete newSubst2;
				return false;
			}*/

			//modify mixture

		}

		bool inorgAcidBase(const InorganicSubstance& subst1, const InorganicSubstance& subst2)
		{
			// check if the reaction can be done
			// TODO: see how this is handled, maybe remove them, maybe set a flag etc...
			if (subst1.amount().asStd() < MOLAR_EXISTANCE_THRESHOLD || subst2.amount().asStd() < MOLAR_EXISTANCE_THRESHOLD)
				return false;

			long long unsigned int lcm1 = tools::lcm(subst1.absCurrentCationCharge() * subst1.cationCount(), subst2.absCurrentAnionCharge() * subst2.anionCount());
			int requiredCat1 = lcm1 / subst1.absCurrentCationCharge();
			int requiredAn2 = lcm1 / subst2.absCurrentAnionCharge();
			if (subst1.absCurrentCationCharge() * subst1.cationCount() == subst2.absCurrentAnionCharge() * subst2.anionCount())
				requiredCat1 = 1, requiredAn2 = 1;

			long long unsigned int lcm2 = tools::lcm(subst2.absCurrentCationCharge() * subst2.cationCount(), subst1.absCurrentAnionCharge() * subst1.anionCount());

			int requiredCat2 = lcm2 / subst2.absCurrentCationCharge();
			int requiredAn1 = lcm2 / subst1.absCurrentAnionCharge();
			if (subst2.absCurrentCationCharge() * subst2.cationCount() == subst1.absCurrentAnionCharge() * subst1.anionCount())
				requiredCat2 = 1, requiredAn1 = 1;

			int requiredSubst1 = tools::lcm(requiredCat1 / subst1.cationCount(), requiredAn1 / subst1.anionCount());
			int requiredSubst2 = tools::lcm(requiredCat2 / subst2.cationCount(), requiredAn2 / subst2.anionCount());


			//TODO : there is a special case where the charge*counts are equal but the lcm is not 1 (CaOH2 + H2SO4), this special case needs to be handled or a new formula is needed
			double product1 = requiredSubst1 * subst1.cationCount() / subst2.absCurrentAnionCharge();
			double product2 = requiredSubst1 * subst1.anionCount() / subst2.absCurrentCationCharge();

			std::cout << "rCat1= " << requiredCat1 << "\nrAn1= " << requiredAn1 << "\nrCat2= " << requiredCat2 << "\nrAn2= " << requiredAn2 << '\n';
			std::cout << requiredSubst1 << ' ' << requiredSubst2 << ' ' << product1 << ' ' << product2 << '\n';

			std::cout << getId(subst1.cation().item, requiredSubst1 * subst1.cationCount() / product1, subst2.anion().item, requiredSubst2 * subst2.anionCount() / product1) << ' ';

			std::cout << getId(subst2.cation().item, requiredSubst2 * subst2.cationCount() / product2, subst1.anion().item, requiredSubst1 * subst1.anionCount() / product2) << ' ';

		}
	}
}



int main()
{
	//std::cout << files::createChecksum("InorganicSubst.csv");

	//ION TEST
	/*data::IonDataTable datac;
	std::cout<<datac.loadFromFile("Atoms.csv").message<<'\n';
	std::cout << datac.getData().size() << '\n';

	chem::InorganicSubstance subst(chem::Ion(datac, "Mg"), util::WithQuantity<chem::Ion>(chem::Ion(datac, "SO4"), 1));

	subst.printName();
	subst.printAbreviation();
	std::cout << subst.isWellFormed();*/

	//INORGANIC SUBST TEST
	/*data::IonDataTable datac;
	std::cout << datac.loadFromFile("Atoms.csv").message << '\n';
	data::SubstanceDataTable datas;
	std::cout << datas.loadFromFile("InorganicSubst.csv").message << '\n';
	chem::Ion::initialize(&datac);
	chem::InorganicSubstance::initialize(&datac, &datas);
	chem::InorganicSubstance subst("H2SO4");
	subst.printName();*/
	{
		//MIXTURE TEST
		data::IonDataTable datac;
		std::cout << datac.loadFromFile("Atoms.csv").message << '\n';
		data::SubstanceDataTable datas;
		std::cout << datas.loadFromFile("InorganicSubst.csv").message << '\n';
		chem::Ion::initialize(&datac);
		chem::InorganicSubstance::initialize(&datac, &datas);
		chem::Mixture mixture1, mixture2(chem::SystemState(1000.0, 760));
		mixture1.add(chem::InorganicSubstance("H2SO4", util::Quantity(util::Mole, 2.0)));
		mixture1.add(chem::InorganicSubstance("NaOH", util::Quantity(util::Mole, 2.0)));
		mixture2.add(chem::InorganicSubstance("LiCl", util::Quantity(util::Mole, 5.0)));
		mixture1.add(chem::InorganicSubstance("H2SO4", util::Quantity(util::Mole, 2.0)));
		mixture1.add(chem::InorganicSubstance("H2SO4", util::Quantity(util::Mole, 2.0)));
		//mixture1.printConstituents();
		//std::cout << "---------------------\n";
		//mixture2.printConstituents();
		//std::cout<<"---------------------\n";
		mixture1.add(mixture2);
		mixture1.printConstituents();
		//std::cout << "---------------------\n";
		//mixture2.printConstituents();
		mixture1.react();

		/*std::cout << "\n__\n";
		chem::react::inorgAcidBase2(chem::InorganicSubstance("H3PO4", util::Quantity(util::Mole, 1.0)), chem::InorganicSubstance("Ca(OH)2", util::Quantity(util::Mole, 1.0)));
		std::cout << "\n__\n";
		chem::react::inorgAcidBase2(chem::InorganicSubstance("Ca(OH)2", util::Quantity(util::Mole, 1.0)), chem::InorganicSubstance("H3PO4", util::Quantity(util::Mole, 1.0)));
		std::cout << "\n__\n";
		chem::react::inorgAcidBase2(chem::InorganicSubstance("Ca(OH)2", util::Quantity(util::Mole, 1.0)), chem::InorganicSubstance("H2SO4", util::Quantity(util::Mole, 1.0)));
		std::cout << "\n__\n";
		chem::react::inorgAcidBase2(chem::InorganicSubstance("H2SO4", util::Quantity(util::Mole, 1.0)), chem::InorganicSubstance("Ca(OH)2", util::Quantity(util::Mole, 1.0)));
		std::cout << "\n__\n";
		chem::react::inorgAcidBase2(chem::InorganicSubstance("NaCl", util::Quantity(util::Mole, 1.0)), chem::InorganicSubstance("HCl", util::Quantity(util::Mole, 1.0)));
		std::cout << "\n__\n";
		chem::react::inorgAcidBase2(chem::InorganicSubstance("NaCl", util::Quantity(util::Mole, 1.0)), chem::InorganicSubstance("H3PO4", util::Quantity(util::Mole, 1.0)));
		std::cout << "\n__\n";
		chem::react::inorgAcidBase2(chem::InorganicSubstance("H3PO4", util::Quantity(util::Mole, 1.0)), chem::InorganicSubstance("NaCl", util::Quantity(util::Mole, 1.0)));
		std::cout << "\n__\n";
		chem::react::inorgAcidBase2(chem::InorganicSubstance("Ca(OH)2", util::Quantity(util::Mole, 1.0)), chem::InorganicSubstance("NaCl", util::Quantity(util::Mole, 1.0)));
		std::cout << "\n__\n";*/
	}
	std::cout<<chem::Substance::instanceCount;

	//TEMP ADJUST TEST
	//std::cout << chem::getAdjustedTemperature(100.0, 750);

	/*util::Quantity q(util::Gram, 123.564);
	std::cout << q.asKilo() << ' ' << q.asMilli() << ' ' << q.asMicro() << '\n';
	q.setAsMicro(1);
	q.addAsKilo(1000000);
	std::cout << q.asString();*/

	/*std::ifstream in("Atoms.csv");
	std::cout<<files::verifyChecksum(in).message;
	in.close();*/

	//data::IonDataTable table(std::string("Atoms.csv"));

	//table.print();


	getchar();
	return 0;
}

