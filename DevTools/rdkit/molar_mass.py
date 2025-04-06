from rdkit import Chem
from rdkit.Chem import Descriptors

def calculate_molar_mass(smiles: str) -> float:
    mol = Chem.MolFromSmiles(smiles)
    if mol is None:
        raise ValueError("Invalid SMILES string.")
    return Descriptors.MolWt(mol)


if __name__ == "__main__":
    
    while True:
        smiles = input("Enter a SMILES string: ")
        try:
            molar_mass = calculate_molar_mass(smiles)
            print(f"Molar mass: {molar_mass:.2f} g/mol.")
        except ValueError as e:
            print(e)
