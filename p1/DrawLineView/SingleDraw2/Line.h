#ifdef _linkList
#endif

template<class T>
class linkList
{
	private:
		T *linkHead;
	public:
		linkList();
		linkList(T *node);
		void append(T *node);
		T* getLinkHead();
		void erase(T* node);
		~linkList();
};

template<class T>
linkList<T>::linkList()
{
	linkHead=NULL;
}

template<class T>
linkList<T>::linkList(T* node)
{
	linkHead=node;
}

template<class T>
void linkList<T>::append(T* node)
{
	T *p=linkHead;
	if(p==NULL)
		linkHead = node;
	else
	{
		while(p->getNext())
			p=p->getNext();
		p->setNext(node);
	}
	node->setNext(NULL);
}

template<class T>
T *linkList<T>::getLinkHead()
{
	return linkHead;
}

template<class T>
void linkList<T>::erase(T* node)
{
	T *p=linkHead;
	if(linkHead==NULL||node==NULL)
		return;

	if(linkHead==node)
	{
		linkHead = p->getNext();
		delete p;
	}
	else
	{
		T *p1=p;
		while(p!=node&&p)
		{
			p1=p;
			p = p->getNext();
		}
		if(p==NULL)
			return;
		p1->setNext(p->getNext());
		delete p;
	}
}

template<class T>
linkList<T>::~linkList()
{
	T *node1 = linkHead,*node2;
	while(node1)
	{
		node2 = node1->getNext();
		delete node1;
		node1=node2;
	}
	linkHead = NULL;
}

class CLine
{
private:
	CPoint p1,p2;
	CLine *next;
public:
	CLine();
	CLine(CPoint &p1,CPoint &p2);
	CLine(CLine &l);
	CPoint GetP1();
	CPoint Getp2();
	void setNext(CLine *n);
	CLine *getNext();
};

