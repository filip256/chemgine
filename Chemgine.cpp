#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <fstream>

#define CHECKSUM_SEED 2957289472

template <class T = int> class StatusCode
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

	long long unsigned int gcd(unsigned int a, unsigned int b)
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

	long long unsigned int lcm(unsigned int a, unsigned int b)
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
	class AtomicData
	{
	public:
		std::string _atomName, _ionName;
		std::vector<int> _charges;
		long double _atomicMass;
		bool _isPolyatomic;

	public:

		AtomicData(const std::string& atomName, const std::string& ionName, const std::vector<int>& charges, const long double atomicMass, const bool isPolyatomic) :
			_atomName(atomName),
			_ionName(ionName),
			_charges(charges),
			_atomicMass(atomicMass),
			_isPolyatomic(isPolyatomic)
		{}
	};

	class IonData
	{
	public:
		std::string _ionName, _abreviation;
		std::vector<int> _charges;
		long double _atomicMass;
		bool _isPolyatomic;

	public:
		IonData(const std::string& abreviation, const std::string& ionName, const std::vector<int>& charges, const long double atomicMass, const bool isPolyatomic) :
			_abreviation(abreviation),
			_ionName(ionName),
			_charges(charges),
			_atomicMass(atomicMass),
			_isPolyatomic(isPolyatomic)
		{}

		IonData(const std::string& id, const AtomicData& atomData) :
			_abreviation(id),
			_ionName(atomData._ionName),
			_charges(atomData._charges),
			_atomicMass(atomData._atomicMass),
			_isPolyatomic(atomData._isPolyatomic)
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

	class AtomicDataTable : public DataTable<AtomicData>
	{
	public:
		AtomicDataTable() : DataTable()
		{
			
		}

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

				if (!_table.emplace(line[0], AtomicData(line[1], line[2], charges, std::stold(line[4]), tools::stringToBool(line[5]))).second)
					return StatusCode<>::BadFormat;
			}
			file.close();

			return StatusCode<>::Ok;
		}

		const AtomicData& getAtom(const std::string& id) const { return _table.at(id); }
	};

	class SubstanceDataTable : public DataTable<SubstanceData>
	{
	public:
		SubstanceDataTable() : DataTable()
		{

		}

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

		const util::WithQuantity<std::string> getCation(const std::string& id) const { return _table.at(id)._cationId; }

		const util::WithQuantity<std::string> getAnion(const std::string& id) const { return _table.at(id)._anionId; }
	};
}

namespace chem
{
	class Ion
	{
		std::string _id;
		const data::IonData _properties;

	public:
		//copies data from the datastore to internal member properties
		//TODO: check if id exists!
		Ion(const data::AtomicDataTable& dataTable, const std::string& id) :
			_properties(id,
				dataTable.getData().at(id)._ionName,
				dataTable.getData().at(id)._charges,
				dataTable.getData().at(id)._atomicMass,
				dataTable.getData().at(id)._isPolyatomic
			)
		{}

		std::string name() const { return _properties._ionName; }
		long double atomicMass() const { return _properties._atomicMass; }
		std::vector<int> charges() const { return _properties._charges; }
		bool isPolyatomic() const { return _properties._isPolyatomic; }
		std::string abreviation() const { return _properties._abreviation; }

		//returns the index in charges list or -1
		int hasCharge(const int charge) const
		{
			std::vector<int>::size_type size = _properties._charges.size();
			for (std::vector<int>::size_type i = 0; i < size; ++i)
				if (_properties._charges[i] == charge)
					return i;
			return -1;
		}

	};

	class Substance
	{
		std::string _name, _abreviation;

	public:
		Substance(const std::string& name, const std::string& abreviation) : _name(name), _abreviation(abreviation) {}

		std::string name() { return _name; }
		std::string abreviation() { return _abreviation; }
	};

	class InorganicSubstance : Substance
	{
		util::WithQuantity<Ion> _cation, _anion;
		int _cationCharge, _anionCharge;
		bool _isWellFormed;

	public:
		InorganicSubstance(const data::SubstanceDataTable& substanceRef, const data::AtomicDataTable& atomicRef, const std::string& id) :
			_cation(util::WithQuantity<Ion>(Ion(atomicRef, substanceRef.getCation(id).item), substanceRef.getCation(id).amount)),
			_anion(util::WithQuantity<Ion>(Ion(atomicRef, substanceRef.getAnion(id).item), substanceRef.getAnion(id).amount)),
			_isWellFormed(true),
			Substance(substanceRef.getData().at(id)._name, id)
		{
			//TODO: resolve charges
		}

		InorganicSubstance(const util::WithQuantity<Ion>& cation, const util::WithQuantity<Ion>& anion) :
			_cation(cation),
			_anion(anion),
			_isWellFormed(true),
			Substance(cation.item.name() + " " + anion.item.name(), cation.item.abreviation() + anion.item.abreviation())
		{
			//check all combinations until a balanced one is found and set the isWellFormed flag
			//needed because ions can have multiple charges
			const std::vector<int>::size_type size1 = _cation.item.charges().size();
			const std::vector<int>::size_type size2 = _anion.item.charges().size();
			bool ok = false;
			for (std::vector<int>::size_type i = 0; i < size1 && !ok; ++i)
				for (std::vector<int>::size_type j = 0; j < size2; ++j)
					if (_cation.amount.asStd() * _cation.item.charges()[i] == -1 * (_anion.amount.asStd() * _anion.item.charges()[j]))
					{
						_cationCharge = _cation.item.charges()[i];
						_anionCharge = _anion.item.charges()[j];
						ok = true;
						break;
					}

			if (!ok)
				_isWellFormed = false;
		}

		bool isWellFormed() const { return _isWellFormed; }

		void printName()
		{
			std::cout << name();
		}
		void printAbreviation()
		{
			std::cout << abreviation();
		}
	};

	class OrganicSubstance : Substance
	{

	};

	class Benzaldehyde : OrganicSubstance
	{

	};



	class Molecule
	{
	public:
	};

	class Mixture
	{
	public:

	};

}


int main()
{
	/*data::AtomicDataTable datac;
	std::cout<<datac.loadFromFile("Atoms.csv").message<<'\n';
	std::cout << datac.getData().size() << '\n';

	chem::InorganicSubstance subst(chem::Ion(datac, "Mg"), util::WithQuantity<chem::Ion>(chem::Ion(datac, "SO4"), 1));

	subst.printName();
	subst.printAbreviation();
	std::cout << subst.isWellFormed();*/

	data::AtomicDataTable datac;
	std::cout << datac.loadFromFile("Atoms.csv").message << '\n';
	data::SubstanceDataTable datas;
	std::cout << datas.loadFromFile("InorganicSubst.csv").message << '\n';
	chem::InorganicSubstance subst(datas, datac, "H2SO4");
	subst.printName();

	/*util::Quantity q(util::Gram, 123.564);
	std::cout << q.asKilo() << ' ' << q.asMilli() << ' ' << q.asMicro() << '\n';
	q.setAsMicro(1);
	q.addAsKilo(1000000);
	std::cout << q.asString();*/

	//std::cout << files::createChecksum("InorganicSubst.csv");

	/*std::ifstream in("Atoms.csv");
	std::cout<<files::verifyChecksum(in).message;
	in.close();*/

	//data::AtomicDataTable table(std::string("Atoms.csv"));

	//table.print();


	getchar();
	return 0;
}

