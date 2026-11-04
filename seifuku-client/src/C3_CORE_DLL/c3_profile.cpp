#include <windows.h>

namespace
{
class C3ProfileNode
{
public:
	C3ProfileNode ( const char* pszName, C3ProfileNode* pParent );
	~C3ProfileNode ();
	void	Reset	( void );

private:
	const char*		m_pszName;
	DWORD			m_dwCount[3];
	DWORD			m_dwStart[2];
	DWORD			m_dwData[12];
	C3ProfileNode*	m_pParent;
	C3ProfileNode*	m_pChild;
	C3ProfileNode*	m_pSibling;
};

C3ProfileNode::C3ProfileNode ( const char* pszName, C3ProfileNode* pParent )
{
	m_pszName = pszName;
	m_pParent = pParent;
	for ( int i = 0; i < 3; i++ )
		m_dwCount[i] = 0;
	for ( int i = 0; i < 12; i++ )
		m_dwData[i] = 0;
	m_pChild = NULL;
	m_pSibling = NULL;
	Reset ();
}

C3ProfileNode::~C3ProfileNode ()
{
	delete m_pChild;
	delete m_pSibling;
}

void C3ProfileNode::Reset ( void )
{
	for ( C3ProfileNode* pNode = this; pNode; pNode = pNode->m_pSibling )
	{
		pNode->m_dwCount[1] = 0;
		pNode->m_dwCount[0] = 0;
		pNode->m_dwCount[2] = 0;
		if ( pNode->m_pChild )
			pNode->m_pChild->Reset ();
	}
}

C3ProfileNode	s_ProfileRoot ( "Root", NULL );
C3ProfileNode*	s_pProfileNode = &s_ProfileRoot;

}
