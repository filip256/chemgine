from rdkit import Chem
from collections import Counter


def count_fundamental_cycles(smiles):
    mol = Chem.MolFromSmiles(smiles)
    if mol is None:
        raise ValueError("Invalid SMILES string.")

    sssr = Chem.GetSymmSSSR(mol)
    sizes = [len(ring) for ring in sssr]
    count_by_size = Counter(sizes)
    return count_by_size


if __name__ == "__main__":
    while True:
        smiles = input("Enter a SMILES string: ")
        try:
            count_by_size = count_fundamental_cycles(smiles)
            for size in sorted(count_by_size):
                print(f"Cycle size {size}: {count_by_size[size]} ring(s).")

        except ValueError as e:
            print(e)
