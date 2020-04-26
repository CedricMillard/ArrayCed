/*
 * Class to manage dynamic arrays
 * (c) Cedric Millard 2020
 * TODO:
 *  - Add a method to find value by a key. Idea is to keep a String with all ideas of each item and determine the Index of the id to find the index of the item
 *  - Or add the ID in the Struct as optional
 * Example:
    Serial.println("test Array");
    Serial.println("Add...");
    Array<String> myArray;
    myArray.add("3");
    myArray.add("4");
    myArray.add("6");
    myArray.add("5", 2);
    myArray.add("2", 0);
    myArray[2] = "A";
    for(int i=0;i<myArray.size(); i++) Serial.println(myArray[i]);
    Serial.println("Remove...");
    myArray.remove(myArray.size()-1);
    myArray.remove(0);
    myArray.remove(1);
    for(int i=0;i<myArray.size(); i++) Serial.println(myArray[i]);
    Serial.println("Remove All...");
    while(myArray.size()) myArray.remove(myArray.size()-1);
    Serial.println("Done");
 */

#ifndef Array_h
#define Array_h
#include <Arduino.h>

template <class T>
struct ArrayItem
{
  T pItem;
  short ID;
  ArrayItem *nextItem = NULL;
  ArrayItem *prevItem = NULL;
};

template <class T>
class Array
{
  public:
    Array(){}
    //Add an item to array, return new arraysize. If iIndex = -1, add at the end. If iIndex = 0 add at the beginning
    int add(T iItem, short iID = -1, int iIndex = -1);
    //Add an item in the growing order
    int addSorted(T iItem, short iID = -1);
    bool exists(short iID);
    int getIndex(short iID);
    T& getItem(short iID);
    T& operator[](int iIndex);
    //Remove and delete an element and return new size of the list
    bool remove(int iIndex);
    bool removeAll();
    bool removeId(short iID);
    int size();

  protected:
    int iSize = 0;
    ArrayItem<T> *pFirst = NULL;
    ArrayItem<T> *pLast = NULL;

};

template <class T>
int Array<T>::add(T iItem, short iID, int iIndex)
{
  ArrayItem<T> *pNewItem = new ArrayItem<T>;
  pNewItem->pItem = iItem;
  pNewItem->ID = iID;
  if(iSize==0)
  {
    pLast = pFirst = pNewItem;
  }
  //Add to the end
  else if(iIndex == -1 || iIndex >= iSize)
  {
    pNewItem->prevItem = pLast;
    if(pLast) pLast->nextItem = pNewItem;
    pLast = pNewItem;
  }
  //Add at the begining of a non empty list
  else if (iIndex == 0)
  {
    pNewItem->nextItem = pFirst;
    if(pFirst) pFirst->prevItem = pNewItem;
    pFirst = pNewItem;
  }
  //Add in the middle
  else
  {
    ArrayItem<T> *pCurrentItem = pFirst;
    //Find the previous item
    for(int i=1;i<iIndex;i++) pCurrentItem = pCurrentItem->nextItem;
    if(pCurrentItem->nextItem->prevItem) pCurrentItem->nextItem->prevItem = pNewItem;
    pNewItem->nextItem = pCurrentItem->nextItem;
    pCurrentItem->nextItem = pNewItem;
  }
  iSize++;
  return iSize;
}

template <class T>
int Array<T>::addSorted(T iItem, short iID)
{
  if(iSize==0) add(iItem, iID);
  int iIndex=0;
  bool bContinue = true;
  ArrayItem<T> *pCurrentItem = pFirst;
  while(iIndex<iSize && bContinue)
  {
    if(iItem>pCurrentItem->pItem)
    {
      iIndex++;
      pCurrentItem = pCurrentItem->nextItem;
    }
    else bContinue = false;
  }
  add(iItem, iID, iIndex);

  return iSize;
}

template <class T>
T& Array<T>::operator[](int iIndex)
{
  T toto;
  if (iIndex>=iSize) return toto;
  if(iIndex==0) return pFirst->pItem;
  ArrayItem<T> *pCurrentItem = pFirst;
  //Find the previous item
  for(int i=1;i<=iIndex && pCurrentItem;i++) pCurrentItem = pCurrentItem->nextItem;
  if(!pCurrentItem) return toto;
  return pCurrentItem->pItem;
}

template <class T>
bool Array<T>::remove(int iIndex)
{
  bool bResult = true;
  //remove first
  if (iIndex>=iSize || iSize==0)
  {
    //Increase size and do nothing as last instruction is to reduce size...
    iSize++;
    bResult = false;
  }
  //Remove First
  else if(iIndex==0)
  {
    ArrayItem<T> *pNext = pFirst->nextItem;
    if(pNext) pNext->prevItem = NULL;
    else pLast = NULL;
    delete pFirst;
    pFirst = pNext;
  }
  //Remove Last
  else if(iIndex==iSize-1)
  {
    ArrayItem<T> *pPrev = pLast->prevItem;
    if(pPrev) pPrev->nextItem = NULL;
    else pFirst = NULL;
    delete pLast;
    pLast = pPrev;
  }
  else
  {
    ArrayItem<T> *pPrevItem = pFirst;
    //Find the previous item
    for(int i=1;i<iIndex;i++) pPrevItem = pPrevItem->nextItem;
    ArrayItem<T> *pItemToDelete = pPrevItem->nextItem;
    ArrayItem<T> *pNextItem = pItemToDelete->nextItem;
    if(pItemToDelete) pPrevItem->nextItem = pNextItem;
    if(pNextItem) pNextItem->prevItem = pPrevItem;
    delete pItemToDelete;
  }
  iSize--;
  return bResult;
}

template <class T>
int Array<T>::getIndex(short iID)
{
  int iIndex = 0;

  if(iSize==0) iIndex = -1;
  else
  {
    ArrayItem<T> *pCurrentItem = pFirst;
    while(pCurrentItem->ID != iID && pCurrentItem->nextItem)
    {
      pCurrentItem = pCurrentItem->nextItem;
      iIndex++;
    }
    if(!pCurrentItem) iIndex = -1;
    else if(pCurrentItem->ID != iID) iIndex = -1;
  }
  return iIndex;
}

template <class T>
bool Array<T>::removeId(short iID)
{
  bool bResult = true;
  int iIndex = getIndex(iID);
  if (iIndex>=0) bResult = remove(iIndex);
  else bResult = false;
  return bResult;
}

template <class T>
bool Array<T>::exists(short iID)
{
  bool bResult = true;
  if(iSize==0) bResult = false;
  else
  {
    ArrayItem<T> *pCurrentItem = pFirst;
    while(pCurrentItem->ID != iID && pCurrentItem->nextItem) pCurrentItem = pCurrentItem->nextItem;
    if(!pCurrentItem) bResult = false;
    else if(pCurrentItem->ID != iID) bResult = false;
  }

  return bResult;
}

template <class T>
T& Array<T>::getItem(short iID)
{
  T toto;
  if(iSize==0) return toto;
  ArrayItem<T> *pCurrentItem = pFirst;
  while(pCurrentItem->ID != iID && pCurrentItem->nextItem) pCurrentItem = pCurrentItem->nextItem;
  if(!pCurrentItem) return toto;
  if(pCurrentItem->ID != iID) return toto;
  return pCurrentItem->pItem;
}

template <class T>
bool Array<T>::removeAll()
{
  bool bResult = true;
  while(iSize) bResult = bResult && remove(0);
  return bResult;
}

template <class T>
int Array<T>::size()
{
  return iSize;
}


#endif
