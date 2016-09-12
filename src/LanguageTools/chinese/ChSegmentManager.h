#ifndef CH_SEGMENT_MANAGER_H
#define CH_SEGMENT_MANAGER_H

//BOOST
#include <boost/noncopyable.hpp>

//MAIN
#include "ChSegmentDef.h"
#include "INTPBCLAS.h"
#include "Common/RWLock.h"

//STL
#include <vector>
#include <map>
#include <string>
#include <set>
#include <fstream>
using namespace std;


class ChSegmentManager
	: private boost::noncopyable
{
public:
	
	~ChSegmentManager();

	static ChSegmentManager * GetInstance() { return &ms_instance; }

	bool Initialize(const string & upload_file_path);

	void Segment(const string & src, string & tgt);
	void Segment(const string & src, string & tgt, const set<string> & force_dict);

	bool InsertSegWord(seg_word_share_ptr sp_word);
	pair<bool, size_t> InsertSegWord(vector<seg_word_share_ptr> & sp_word_vec);
	pair<bool, size_t> InsertSegWord(const string & file_name, const int is_active, const bool is_utf8);
	bool DeleteSegWord(const int word_id);
	size_t DeleteSegWord(const vector<int> & word_id_vec);
	bool ModifySegWord(seg_word_share_ptr sp_word);
	size_t RecoverySegWord(const vector<int> & word_id_vec);
	
private:
	static ChSegmentManager ms_instance;

private:
	ChSegmentManager();

	RWLock m_rw_lock;
	CINTPBCLAS m_ch_pbclas;    //ÖÐÎÄ·Ö´Ê pbclas

	map<string, seg_word_share_ptr> m_word_map;
	map<int, seg_word_share_ptr> m_word_id_map;
	
	string m_upload_file_path;



};

#endif //CH_SEGMENT_MANAGER_H
