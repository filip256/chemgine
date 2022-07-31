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

namespace data
{
	class IonData
	{
	public:
		std::string _ionName, _abreviation;
		std::vector<int> _charges;
		double _atomicMass;
		bool _isPolyatomic;

	public:
		IonData(const std::string& abreviation, const std::string& ionName, const std::vector<int>& charges, const double atomicMass, const bool isPolyatomic) :
			_abreviation(abreviation),
			_ionName(ionName),
			_charges(charges),
			_atomicMass(atomicMass),
			_isPolyatomic(isPolyatomic)
		{}
	};

	class AtomicData
	{
	public:
		std::string _atomName, _ionName;
		std::vector<int> _charges;
		double _atomicMass;
		bool _isPolyatomic;

	public:
		AtomicData() :
			_atomName("empty"),
			_ionName("empty"),
			_atomicMass(0),
			_isPolyatomic(false)
		{}

		AtomicData(const std::string& atomName, const std::string& ionName, const std::vector<int>& charges, const double atomicMass, const bool isPolyatomic) :
			_atomName(atomName),
			_ionName(ionName),
			_charges(charges),
			_atomicMass(atomicMass),
			_isPolyatomic(isPolyatomic)
		{}
	};

	class AtomicDataTable
	{
		std::map<std::string, AtomicData> _table;
	public:
		AtomicDataTable() : _table() 
		{
			
		}

		const StatusCode<> loadFromFile(const std::string& dataFile)
		{
			std::ifstream file(dataFile);

			if (!file.is_open())
				return StatusCode<>::FileCouldNotOpen;

			if (files::verifyChecksum(file).code != 200) //not OK
				return StatusCode<>::FileCorrupted;

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

				_table[line[0]] = AtomicData(line[1], line[2], charges, std::stod(line[4]), tools::stringToBool(line[5]));
			}
			file.close();

			return StatusCode<>::Ok;
		}

		const std::map<std::string, AtomicData>& getData() const { return _table; }

		void print()
		{
			//mostly for testing
			for (std::map<std::string, AtomicData>::iterator it = _table.begin(), end = _table.end(); it != end; ++it)
			{
				std::cout << it->first << ": " << it->second._atomName << ", " << it->second._ionName << ", [";
				unsigned int size = it->second._charges.size();
				for (unsigned int i = 0; i < size; ++i)
					std::cout << it->second._charges[i] << ",";
				std::cout<< "], " << it->second._atomicMass << ", " << it->second._isPolyatomic << '\n';
			}

		}
	};
}

namespace chem
{
	class Quantity
	{
		//store in standard measure unit (i.e. grams, moles)
		long double _standardAmount;

	public:
		Quantity() :
			_standardAmount(0.0)
		{}

		Quantity(const long double amount) :
			_standardAmount(amount)
		{}

		long double asKilo() const { return _standardAmount / 1000.0; }
		long double asStd() const { return _standardAmount; }
		long double asMilli() const { return _standardAmount * 1000.0; }
		long double asMicro() const { return _standardAmount * 1000000.0; }
		std::string asString() const { return std::to_string(_standardAmount); }

		void setAsKilo(const long double amount) { _standardAmount = amount * 1000.0; }
		void set(const long double amount) { _standardAmount = amount; }
		void setAsMilli(const long double amount) { _standardAmount = amount / 1000.0; }
		void setAsMicro(const long double amount) { _standardAmount = amount / 1000000.0; }

		void addAsKilo(const long double amount) { _standardAmount += amount * 1000.0; }
		void add(const long double amount) { _standardAmount += amount; }
		void addAsMilli(const long double amount) { _standardAmount += amount / 1000.0; }
		void addAsMicro(const long double amount) { _standardAmount += amount / 1000000.0; }

	};

	template <class A, class B> class Pair
	{
	public:
		A a;
		B b;
	};

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

		std::string name()        const { return _properties._ionName;     }
		std::string abreviation() const { return _properties._abreviation; }

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
		Ion _cation, _anion;

	public:
		InorganicSubstance(const Ion& cation, const Ion& anion) :
			_cation(cation),
			_anion(anion),
			Substance(cation.name() + " " + anion.name(), cation.abreviation() + anion.abreviation())
		{}



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

	chem::InorganicSubstance subst(chem::Ion(datac, "Na"), chem::Ion(datac, "Cl"));

	subst.printName();
	subst.printAbreviation();*/

	chem::Quantity q(123.564);
	std::cout << q.asKilo() << ' ' << q.asMilli() << ' ' << q.asMicro() << '\n';
	q.setAsMicro(1);
	q.addAsKilo(1000000);
	std::cout << q.asString();

	//std::cout << files::createChecksum("Atoms.csv");

	/*std::ifstream in("Atoms.csv");
	std::cout<<files::verifyChecksum(in).message;
	in.close();*/

	//data::AtomicDataTable table(std::string("Atoms.csv"));

	//table.print();



	getchar();
	return 0;
}

