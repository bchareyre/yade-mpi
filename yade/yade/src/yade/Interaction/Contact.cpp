#include "Contact.hpp"

Contact::Contact () : Interaction()
{
}

Contact::Contact (int i1,int i2) : Interaction()
{
	id1 = i1;
	id2 = i2;
}

Contact::~Contact ()
{

}
