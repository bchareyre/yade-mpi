/***************************************************************************
 *   Copyright (C) 2004 by Olivier Galizzi                                 *
 *   olivier.galizzi@imag.fr                                               *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
 
#ifndef __TREE_H__
#define __TREE_H__

#include <vector>
#include <list>

template<class NodeDataType>
class Tree 
{	
	typedef struct {
		NodeDataType data;
		std::vector<Node*> childs;
		Node * father;
		Node * next;
	} Node;
	
	std::vector<Node*> levels;
	
	// construction
	public : Tree<NodeDataType> ()
	{
		levels.resize(1);
		Node * root;
		root->father=NULL;
		root->next=NULL;
		root->childs.resize(0);
		levels[0]=root;
	};
	
	public : virtual ~Tree<NodeDataType>() {};
	
	public : inline Node* root()
	{
		return levels[0];
	}
	
	public : inline Node* level(int i)
	{
		//assert();
		return levels[i];
	}
	
	public : inline void addChild(Node* father,NodeDataType data)
	{
		Node * son = new Node(data);
		son->father=father;
		int i=1;
		Node * n = son;
		while (n->father!=levels[0])
		{
			n = n->father;
			i++;
		}		
		if (i<levels.size())
		{
			levels->resize(i+1);
			levels[i]=NULL;
		}
		son->next = levels[i];
		levels[i] = son;
		father->childs.push_back(son);
	}

};

#endif // __TREE_H__
