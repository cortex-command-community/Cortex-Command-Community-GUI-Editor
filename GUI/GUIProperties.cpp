//////////////////////////////////////////////////////////////////////////////////////////
// File:            GUIProperties.cpp
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     GUIProperties class
// Project:         GUI Library
// Author(s):       Jason Boettcher
//                  jackal@shplorb.com
//                  www.shplorb.com/~jackal


//////////////////////////////////////////////////////////////////////////////////////////
// Inclusions of header files

#include "GUI.h"

using namespace RTE;

//////////////////////////////////////////////////////////////////////////////////////////
// Constructor:     GUIProperties
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Constructor method used to instantiate a GUIProperties object in 
//                  system memory.

GUIProperties::GUIProperties(const std::string Name)
{
    m_Name = Name;
    m_VariableList.clear();
}


//////////////////////////////////////////////////////////////////////////////////////////
// Constructor:     GUIProperties
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Constructor method used to instantiate a GUIProperties object in 
//                  system memory.

GUIProperties::GUIProperties()
{
    m_Name = "";
    m_VariableList.clear();
}


//////////////////////////////////////////////////////////////////////////////////////////
// Destructor:        GUIProperties
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Destructor method used to free a GUIProperties object in system
//                  memory.

GUIProperties::~GUIProperties()
{
    Clear();
}


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          Clear
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Clears the properties.

void GUIProperties::Clear(void)
{
    // Free the list
	std::vector <PropVariable *>::iterator it;

    for(it = m_VariableList.begin(); it != m_VariableList.end(); it++) {
        PropVariable *p = *it;

        // Free the property
        delete p;
    }

    m_VariableList.clear();
}


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          AddVariable
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Adds a new variable to the properties

void GUIProperties::AddVariable(const std::string Variable, const std::string Value)
{
    // If this property already exists, just update it
	std::string Val;
    if (GetValue(Variable, &Val)) {
        SetValue(Variable, Value);
        return;
    }

    PropVariable *Prop = new PropVariable;
    
    Prop->m_Name = Variable;
    Prop->m_Value = Value;

    m_VariableList.push_back(Prop);
}


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          AddVariable
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Adds a new variable to the properties

void GUIProperties::AddVariable(const std::string Variable, char *Value)
{
	std::string Val = Value;

    AddVariable(Variable, Val);
}


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          AddVariable
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Adds a new variable to the properties

void GUIProperties::AddVariable(const std::string Variable, int Value)
{
    char buf[32];
    sprintf_s(buf, sizeof(buf), "%i", Value);
	std::string Val(buf);
    AddVariable(Variable, Val);
}


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          AddVariable
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Adds a new variable to the properties

void GUIProperties::AddVariable(const std::string Variable, bool Value)
{
	std::string Val = Value ? "True" : "False";
    AddVariable(Variable, Val);
}


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          SetValue
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Changes the value of a property.

bool GUIProperties::SetValue(const std::string Variable, const std::string Value)
{
    // Find the property
	std::vector <PropVariable *>::iterator it;

    for(it = m_VariableList.begin(); it != m_VariableList.end(); it++) {
        PropVariable *p = *it;

        // Matching name?
        if (_stricmp(p->m_Name.c_str(), Variable.c_str()) == 0) {
            p->m_Value = Value;
            return true;
        }
    }

    // Not set
    return false;
}


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          SetValue
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Changes the value of a property.

bool GUIProperties::SetValue(const std::string Variable, int Value)
{
    char buf[64];
    sprintf_s(buf, sizeof(buf), "%i", Value);

    return SetValue(Variable, buf);
}


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          Update
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Updates the properties with properties from another instance.

void GUIProperties::Update(GUIProperties *Props, bool Add)
{
    assert(Props);

	std::vector <PropVariable *>::iterator it1;

    for(it1 = Props->m_VariableList.begin(); it1 != Props->m_VariableList.end(); it1++) {
        PropVariable *Src = *it1;

        // Set the variable
        if (!SetValue(Src->m_Name, Src->m_Value) && Add)
            AddVariable(Src->m_Name, Src->m_Value);
    }

}


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          GetValue
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Gets a string value

bool GUIProperties::GetValue(const std::string Variable, std::string *Value)
{
    // Find the property
	std::vector <PropVariable *>::iterator it;

    for(it = m_VariableList.begin(); it != m_VariableList.end(); it++) {
        PropVariable *p = *it;

        // Matching name?
        if (_stricmp(p->m_Name.c_str(), Variable.c_str()) == 0) {
            *Value = p->m_Value;
            return true;
        }
    }

    // Not found
    return false;
}


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          GetValue
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Gets a string array of values

int GUIProperties::GetValue(const std::string Variable, std::string *Array, int MaxArraySize)
{
    assert(Array);

	std::string Value;

    // Get the string
    if (!GetValue(Variable, &Value))
        return 0;
    
    // Create a c string version of the value for tokenizing
    char *str = new char[Value.length()+1];
    if (!str)
        return 0;

    // Tokenize the string
    strcpy(str, Value.c_str());
    char *tok = strtok(str, ",");
    int count = 0;
    
    while(tok && count < MaxArraySize) {

        Array[count++] = GUIUtil::TrimString(tok);

        tok = strtok(0, ",");
    }

    delete[] str;

    // Return the number of elements read
    return count;

}


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          GetValue
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Gets an integer array of values

int GUIProperties::GetValue(const std::string Variable, int *Array, int MaxArraySize)
{
    assert(Array);

	std::string Value;

    // Get the string
    if (!GetValue(Variable, &Value))
        return 0;
    
    // Create a c string version of the value for tokenizing
    char *str = new char[Value.length()+1];
    if (!str)
        return 0;

    // Tokenize the string
    strcpy(str, Value.c_str());
    char *tok = strtok(str, ",");
    int count = 0;
    
    while(tok && count < MaxArraySize) {

        Array[count++] = atoi(tok);

        tok = strtok(0, ",");
    }

    delete[] str;

    // Return the number of elements read
    return count;
}


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          GetValue
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Gets a single interger

bool GUIProperties::GetValue(const std::string Variable, int *Value)
{
    assert(Value);

	std::string val;

    // Get the string
    if (!GetValue(Variable, &val))
        return false;

    *Value = atoi(val.c_str());

    // Found the value
    return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
// Method:          GetValue
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Gets a single unsigned interger

bool GUIProperties::GetValue(const std::string Variable, unsigned long *Value)
{
    assert(Value);

	std::string val;

    // Get the string
    if (!GetValue(Variable, &val))
        return false;

    *Value = atol(val.c_str());

    // Found the value
    return true;
}


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          GetValue
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Gets a boolean value

bool GUIProperties::GetValue(const std::string Variable, bool *Value)
{
    assert(Value);

	std::string val;

    // Get the string
    if (!GetValue(Variable, &val))
        return false;

    // Default is false
    *Value = false;

    // Convert the string into a boolean
    if (_stricmp(val.c_str(), "true") == 0)
        *Value = true;

    // Found the value
    return true;
}


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          GetName
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Gets the property name

std::string GUIProperties::GetName(void)
{
    return m_Name;
}


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          ToString
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Converts the properties to a string

std::string GUIProperties::ToString(void)
{
	std::string OutString = "";

    // Go through each value
	std::vector <PropVariable *>::iterator it;
    for(it = m_VariableList.begin(); it != m_VariableList.end(); it++) {
        PropVariable *V = *it;

        OutString += V->m_Name;
        OutString.append(" = ");
        OutString += V->m_Value;
        OutString.append("\n");
    }

    return OutString;
}


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          GetCount
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Gets the variable count in the properties

int GUIProperties::GetCount(void)
{
    return m_VariableList.size();
}


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          GetVariable
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Gets a variable based on index

bool GUIProperties::GetVariable(int Index, std::string *Name, std::string *Value)
{
    // Check for a bad index
    if (Index < 0 || Index >= m_VariableList.size())
        return false;

    PropVariable *P = (PropVariable *)m_VariableList.at(Index);
    if (Name)
        *Name = P->m_Name;

    if (Value)
        *Value = P->m_Value;

    return true;
}


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          SetVariable
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Sets a variable based on index

bool GUIProperties::SetVariable(int Index, std::string Name, std::string Value)
{
    // Check for a bad index
    if (Index < 0 || Index >= m_VariableList.size())
        return false;

    PropVariable *P = (PropVariable *)m_VariableList.at(Index);
    P->m_Name = Name;
    P->m_Value = Value;

    return true;
}


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          Sort
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Sorts

void GUIProperties::Sort(bool Ascending)
{
    // Simple bubble sort
    
    for(int i=0; i<m_VariableList.size(); i++) {

        for(int j=0; j<m_VariableList.size()-1-i; j++) {

            PropVariable *V = (PropVariable *)m_VariableList.at(j);
            PropVariable *V2 = (PropVariable *)m_VariableList.at(j+1);

            if ((V->m_Name.compare(V2->m_Name) > 0 && Ascending) ||
               (V->m_Name.compare(V2->m_Name) < 0 && !Ascending)) {

                // Swap em
                PropVariable temp = *V;                
                *V = *V2;
                *V2 = temp;
            }
        }
    }
}
