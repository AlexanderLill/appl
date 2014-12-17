#include "BeliefTreeNode.h"

BeliefTreeNode::BeliefTreeNode(void)
{
	s = NULL;
}

BeliefTreeNode::~BeliefTreeNode(void)
{
}
void BeliefTreeNode::print()
{
	cout << "BeliefTreeNode - cacheindex: row=" << cacheIndex.row << " sval=" << cacheIndex.sval << " belief=";
	s->bvec->write(cout);
	cout << " sval=" << s->sval << endl;

	for(vector<BeliefTreeQEntry>::iterator iter1 = Q.begin() ; iter1 != Q.end() ; iter1 ++)
	{
	    cout << "ImmReward=" << iter1->immediateReward << " lbVal=" << iter1->lbVal << " (Row=" << cacheIndex.row << ")" << endl;
//		BeliefTreeQEntry&  entry= *iter1;
//		for(vector<BeliefTreeObsState*>::iterator iter2 = entry.stateOutcomes.begin() ; iter2 != entry.stateOutcomes.end() ; iter2 ++)
//		{
//			BeliefTreeObsState* obsState = *iter2;
//			if( obsState == NULL)
//			{
//				continue;
//			}
//
//			for(vector<BeliefTreeEdge*>::iterator iter3 = obsState->outcomes.begin() ; iter3 != obsState->outcomes.end() ; iter3 ++)
//			{
//				BeliefTreeEdge* edge = *iter3;
//				if(edge!= NULL && edge->nextState != NULL)
//				{
//				    if (this != edge->nextState) {
//                        edge->nextState->print();
//				    } else {
//                        cout << "Avoided recursion in BeliefTreeNode::print()" << endl;
//                    }
//
//				}
//			}
//
//		}
	}
}



BeliefTreeEdge::BeliefTreeEdge()
{
	nextState = NULL;
}
BeliefTreeEdge::~BeliefTreeEdge()
{

}
