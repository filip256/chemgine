#pragma once

#include <vector>
#include <string>
#include <unordered_map>
#include <stack>
#include "Bond.hpp"

class MolecularStructure
{
private:
    std::vector<std::reference_wrapper<BaseComponent>> nodes;
    std::vector<std::vector<std::reference_wrapper<Bond>>> edges;

public:
    //MolecularStructure(const std::string& smiles)
    //{
    //    size_t i = 0;
    //    std::unordered_map<unsigned short int, size_t> rings;
    //    std::stack<int> branches;

    //    while (i < smiles.size())
    //    {
    //        int prev = -1;
    //        if (nodes.empty() == false)
    //            prev = nodes.size() - 1;

    //        while (smiles[i] == ')') 
    //        {
    //            prev = branches.top();
    //            branches.pop();
    //            ++i;
    //            if (i == smiles.size()) break;
    //        }

    //        if (isalpha(smiles[i]) || smiles[i] == '[') 
    //        {
    //            size_t copyStrpos = i;
    //            if (isalpha(i)) 
    //            { // normal atom
    //                if (i + 1 < smiles.size() && isalpha(smiles[i + 1]) && periodicTable.containsKey(smiles.substr(strpos, 2)))
    //                { // two-letter element
    //                    curr->setToken(smiles.substr(strpos, 2));
    //                }
    //                else 
    //                { // one-letter element
    //                    curr->setToken(charToString(smiles[strpos]));
    //                }
    //            }
    //            else 
    //            { // SPECIAL ATOM (isotope, chiral, stereo...)
    //                size_t temp = smiles.find(']', i);
    //                std::string token = smiles.substr(i + 1, temp - i - 1);
    //                curr->setToken(token);
    //                i = temp;
    //            }
    //            curr->setAllSpecials();
    //            atoms.add(curr);
    //            if (prev != -1) { // if not the first atom
    //                Bond* bond = new Bond(prevAtom, curr);
    //                if (smiles[copyStrpos - 1] == '-' ||
    //                    smiles[copyStrpos - 1] == '=' ||
    //                    smiles[copyStrpos - 1] == '#') {
    //                    bond->setOrder(smiles[copyStrpos - 1]);
    //                }
    //                bonds.add(bond);
    //            }
    //        }
    //        else if (isDigit(smiles[strpos]) || smiles[strpos] == '%') { // RING CLOSURES
    //            int ringClosure;
    //            if (smiles[strpos] == '%') {
    //                std::string temp;
    //                while (isDigit(smiles[strpos + 1])) {
    //                    temp += smiles[strpos + 1];
    //                    strpos++;
    //                }
    //                ringClosure = stringToInteger(temp);
    //            }
    //            else {
    //                ringClosure = charToInteger(smiles[strpos]);
    //            }
    //            // if the ringClosure already exists --> finish the closure
    //            if (ringClosures.containsKey(ringClosure)) {
    //                Bond* bond = new Bond(ringClosures[ringClosure], atoms.back());
    //                if (smiles[strpos - 1] == '-' ||
    //                    smiles[strpos - 1] == '=' ||
    //                    smiles[strpos - 1] == '#') {
    //                    bond->setOrder(smiles[strpos - 1]);
    //                }
    //                bonds.add(bond);
    //            }
    //            else {
    //                ringClosures.add(ringClosure, atoms.back());
    //            }
    //        }
    //        else if (smiles[strpos] == '(') {
    //            branches.push(atoms.back());
    //        }
    //        strpos++;
    //    }
    //}
};