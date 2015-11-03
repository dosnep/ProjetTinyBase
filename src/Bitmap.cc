#include "rm.h"


Bitmap :: Bitmap(int taille)
{
	this->taille = taille;
	this->tabBitmap = new int[taille];
	int i;
	for(i = 0; i<taille;i++)
	{
		this->tabBitmap[i] = 0;
	}
	
};

Bitmap :: ~Bitmap()
{
	delete [] this->tabBitmap;
	this->tabBitmap = NULL;
	
};

//Retourne vrai s'il n'y a plus de slots vides dans le bitmap sinon faux
bool Bitmap :: IsFull()
{
	int compt = 0;
	int i;
	for(i = 0;i<this->taille;i++)	//On compte le nb de bits à 1 dans le bitmap
	{
		if(this->tabBitmap[i] == 1)
			compt++;
	
	}
	
	if(compt!=this->taille)	//On teste si le nb de bits à 1 est le même que la taille du bitmap
		return false;
	
	
	return true;
};

//slotnum prend le premier slot vide dans le bitmap
RC Bitmap :: GetFirstFree(SlotNum &slotnum) 
{
	
	if(this->IsFull())
		return BITMAP_NO_FREE_SLOT;
		
	int i;
	for(i = 0; i<this->taille;i++)	//On parcours le bitmap et on cherche le premier slot où le bit = 0
	{
		if(this->tabBitmap[i] == 0)
		{
			slotnum = i;
			return 0;
			
		}
	}
		
	return 0;
};

//Retourne le prochain slot non vide à partir de currentSlotNum+1
RC Bitmap :: GetNextSlot(const SlotNum &currentSlotNum,SlotNum &nextSlotNum)
{
	//ON teste si nous sommes à la fin du bitmap
	if(currentSlotNum == this->taille-1)
		return BITMAP_EOF;
	
	//On parcours le bitmap et on cherche le prochain slot occupé
	int i;
	for(i = currentSlotNum+1; i<this->taille; i++)
	{

		if(this->tabBitmap[i] == 1)
		{
			nextSlotNum = i;
			return 0;
		}
	}
	
	//Si nous n'avons pas trouvé de slot
	return BITMAP_NO_NEXT_SLOT;
};

//le bit du slot slotNum prend la valeur value
RC Bitmap :: SetSlot(const SlotNum &slotNum, const int &value)
{
	this->tabBitmap[slotNum] = value;
	return 0;
};


