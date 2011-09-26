/************************************************************************/
/* author: xy                                                           */
/* date: 20110926                                                       */
/************************************************************************/
#ifndef __CACHE_MANAGER_H_310C134F_844C_4590_A4D2_AD30165AF10A__
#define __CACHE_MANAGER_H_310C134F_844C_4590_A4D2_AD30165AF10A__

#include "RTS.h"

class CWordCacheKey
{
public:
    CWordCacheKey(const CWord& word);
    CWordCacheKey(const CWordCacheKey& key);
    CWordCacheKey(const STSStyle& style, const CStringW& str, int ktype, int kstart, int kend);
    bool operator==(const CWordCacheKey& key)const;
    bool operator==(const CWord& key)const;

    FwStringW m_str;
    FwSTSStyle m_style;
    int m_ktype, m_kstart, m_kend;
};

class OverlayKey: public CWordCacheKey
{
public:
    OverlayKey(const CWord& word, const POINT& p, const POINT& org):CWordCacheKey(word),m_p(p),m_org(org) { }
    OverlayKey(const OverlayKey& key):CWordCacheKey(key),m_p(key.m_p),m_org(key.m_org) { }
    OverlayKey(const STSStyle& style, const CStringW& str, int ktype, int kstart, int kend, const POINT& p, const POINT& org)
        :CWordCacheKey(style, str, ktype, kstart, kend),m_p(p), m_org(org) { }
    bool operator==(const OverlayKey& key)const 
    { 
        return ((CWordCacheKey)(*this)==(CWordCacheKey)key) && (m_p.x==key.m_p.x) && (m_p.y==key.m_p.y) 
          && (m_org.x==key.m_org.x) && (m_org.y==key.m_org.y); 
    }    
    bool CompareTo(const CWord& word, const POINT& p, const POINT& org)const 
    { 
        return ((CWordCacheKey)(*this)==word) && (m_p.x==p.x) && (m_p.y==p.y) && (m_org.x==org.x) && (m_org.y==org.y);
    }

    POINT m_p, m_org;    
};

std::size_t hash_value(const CWord& key);
std::size_t hash_value(const OverlayKey& key);
std::size_t hash_value(const CWordCacheKey& key);

//shouldn't use std::pair, or else VC complaining error C2440
//typedef std::pair<OverlayKey, SharedPtrOverlay> OverlayMruItem; 
struct OverlayMruItem
{
    OverlayMruItem(const OverlayKey& overlay_key_, const SharedPtrOverlay& overlay_):overlay_key(overlay_key_),overlay(overlay_){}

    OverlayKey overlay_key;
    SharedPtrOverlay overlay;
};

struct CWordMruItem
{
    CWordMruItem(const CWordCacheKey& word_key_, const SharedPtrCWord& word_):word_key(word_key_),word(word_){}

    CWordCacheKey word_key;
    SharedPtrCWord word;
};

typedef mru_list<
    OverlayMruItem, 
    boost::multi_index::member<OverlayMruItem, 
    OverlayKey, 
    &OverlayMruItem::overlay_key
    >
> OverlayMruCache;

typedef mru_list<
    CWordMruItem, 
    boost::multi_index::member<CWordMruItem, 
    CWordCacheKey, 
    &CWordMruItem::word_key
    >
> CWordMruCache;

class CacheManager
{
public:
    static const int OVERLAY_CACHE_ITEM_NUM = 256;
    static const int WORD_CACHE_ITEM_NUM = 512;

    static OverlayMruCache* GetOverlayMruCache();
    static CWordMruCache* GetCWordMruCache();
private:
    static OverlayMruCache* s_overlay_mru_cache;
    static CWordMruCache* s_word_mru_cache;
};

#endif // end of __CACHE_MANAGER_H_310C134F_844C_4590_A4D2_AD30165AF10A__
