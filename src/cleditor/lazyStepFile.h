#ifndef LAZYSTEPFILE_H
#define LAZYSTEPFILE_H

#include <istream>
#include <map>
#include <vector>
#include <string>

#include "sdaiApplication_instance.h"
#include "STEPfileCommon.h"

typedef long instanceNum;

// typedefs for instanceRefMMap, instanceTypeMMap, instancesLoaded, instanceStreamPosMap
typedef std::multimap< instanceNum, instanceNum > instanceRefMMap_t;
typedef std::pair< instanceNum, instanceNum > instanceRefMMap_pair;
typedef std::pair< instanceRefMMap_t::iterator, instanceRefMMap_t::iterator > instanceRefMMap_range;
typedef std::multimap< std::string, instanceNum > instanceTypeMMap_t;
typedef std::pair< std::string, instanceNum > instanceTypeMMap_pair;
typedef std::pair< instanceTypeMMap_t::iterator, instanceTypeMMap_t::iterator > instanceTypeMMap_range;
typedef std::map< instanceNum, SDAI_Application_instance * > instancesLoaded_t;
typedef std::pair< instanceNum, SDAI_Application_instance * > instancesLoaded_pair;
typedef std::map< instanceNum, std::pair< std::streampos, std::streampos > > instanceStreamPosMap_t;
typedef std::pair< instanceNum, std::pair< std::streampos, std::streampos > > instanceStreamPosMap_pair;

/**
 * lazy loading for Part 21 files
 * somewhat similar to STEPfile class, but:
 *  * it only loads what is necessary
 *  * cannot append
 *  * cannot write
 *  * emulates InstMgr class
 */
class lazyStepFile: public STEPfileCommon {
protected:
//     std::istream in; //in base class
    unsigned long loadedInstances, totalInstances;
    bool completelyLoaded;

    /** multimap from instance number to instances that refer to it
     * \sa instanceRefMMap_pair
     * \sa instanceRefMMap_range
     */
    instanceRefMMap_t  instanceRefMMap;

    /** multimap from instance type to instance number
     * \sa instanceTypeMMap_pair
     * \sa instanceTypeMMap_range
     */
    instanceTypeMMap_t instanceTypeMMap;

    /** map from instance number to instance pointer (loaded instances only)
     * \sa instancesLoaded_pair
     */
    instancesLoaded_t instancesLoaded;

    /** map from instance number to beginning and end positions in the stream
     * \sa instanceStreamPos_pair
     */
    instanceStreamPosMap_t instanceStreamPosMap;


public:
    lazyStepFile( Registry& r, std::ifstream stream ); //lazy loading won't work with a stream that isn't a file, will it? need seekg and tellg
    ~lazyStepFile(); //TODO needs to delete all instances in instancesLoaded; ifndef NDEBUG, warn when deleting if map not empty?

    float getPercentLoaded() {
        return loadedInstances * 100.0 / totalInstances;
    }

    /// remove all SDAI_Application_instance* from instancesLoaded, without deleting the objects
    removeAll() {
        instancesLoaded.clear();
    }
    removeOne( const SDAI_Application_instance * i ); ///< \sa removeAll()

    deleteAll(); ///< delete each SDAI_Application_instance* in instancesLoaded, then clear the map
    deleteOne( SDAI_Application_instance * i ); ///< \sa deleteAll();

    loadAll(); ///< load all instances from the file, if they aren't already.
    loadAll( const std::string type ); ///< load all instances that are an exact match for 'type'. TODO: how to handle complex types?
    loadOne( const instanceNum n ); ///< load instance n if it isn't already

    SDAI_Application_instance * getInstance( const instanceNum n ); ///< if necessary, calls loadOne()
    instanceNum getInstanceNum( const SDAI_Application_instance * i ) const; ///< return -1 if not found

    instanceRefMMap_range getReferentInstanceNums( const SDAI_Application_instance * i ) const {
        return getReferentInstanceNums( getInstanceNum( i ) );
    }
    instanceRefMMap_range getReferentInstanceNums( const instanceNum n ) const {
        return instanceRefMMap.equal_range(n);
    }

    instanceTypeMMap_range getInstanceNumsFromType( const std::string type ) const {
        return instanceTypeMMap.equal_range(type);
    }

    const std::string getInstanceTypeFromNum( const instanceNum n ) const;

    instanceTypeMMap_t::size_type getNumTypes() const {
        // http://www.daniweb.com/software-development/cpp/threads/384836/multimap-and-counting-number-of-keys#post1657899
        instanceTypeMMap_t::size_type n = 0 ;
        instanceTypeMMap_t::iterator iter = instanceTypeMMap.begin();
        for( ; iter != instanceTypeMMap.end(); iter = instanceTypeMMap.upper_bound( iter->first )  ) {
            ++n;
        }
        return n ;
    }

    /** populate an InstMgr.
     * \param onlyLoaded if true, only puts already-loaded instances in the InstMgr
     * \param removeAll WARNING: calling with this set to false and then
     * deleting the object will result in invalid pointers in the InstMgr!
     */
    fillInstMgr(InstMgr & im, bool onlyLoaded = false, bool removeAll = true);
};

#endif //LAZYSTEPFILE_H