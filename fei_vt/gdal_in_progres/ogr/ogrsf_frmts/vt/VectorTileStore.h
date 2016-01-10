#ifndef __VectorTileStore_H_
#define __VectorTileStore_H_



#define	JCLASS_NAME "vectorDataStorge/VectorDataStorge"
#define DLL_API __declspec(dllexport) 
 
#include <string.h>  
#include <stdio.h>
#include <string>
#include <fstream>
#include <io.h>
class DLL_API VectorTileStore
{
public:
	/*
 * @Method:    connect
 * @FullName:  vectorTileStore::connect
 * @Access:    public 
 * @Returns:   bool false失败 true成功
 * @purpose:	  
 */
	bool connect(int argc, char ** argv);
	bool connect();
/*
 * @Method:    disconnect
 * @FullName:  vectorTileStore::disconnect
 * @Access:    public 
 * @Returns:   bool false失败 true成功
 * @purpose:	  
 */
	bool disConnect();
/*
 * @Method:    getTile
 * @FullName:  vectorTileStore::getTile
 * @Access:    public 
 * @Returns:   int > 0瓦片的长度 = 0 没有该瓦片  <0 出现错误
 * @Parameter: const char * key
 * @Parameter: const char * type
 * @Parameter: const char * scale
 * @Parameter: const char * dataStandard
 * @Parameter: char * * tileValue
 * @purpose:	  
 */
	int getTile( const char* key, const char*type, const char* scale, const char* dataStandard, char** tileValue );
/*
 * @Method:    putTile
 * @FullName:  vectorTileStore::putTile
 * @Access:    public 
 * @Returns:   int -1失败 0成功
 * @Parameter: const char * key
 * @Parameter: const char * type
 * @Parameter: const char * scale
 * @Parameter: const char * dataStandard
 * @Parameter: const char * tileValue
 * @Parameter: int tileLength 
 * @purpose:	  
 */
	int putTile( const char* key, const char* type, const char* scale, const char* dataStandard, const char* tileValue, int tileLength );
/*
 * @Method:    deleteTile
 * @FullName:  vectorTileStore::deleteTile
 * @Access:    public -1失败 0成功
 * @Returns:   int
 * @Parameter: const char * key
 * @Parameter: const char * type
 * @Parameter: const char * scale
 * @Parameter: const char * dataStandard
 * @purpose:	  
 */
	int deleteTile( const char* key, const char*type, const char* scale, const char* dataStandard );
/*
 * @Method:    flush
 * @FullName:  vectorTileStore::flush
 * @Access:    public 
 * @Returns:   void
 * @purpose:	  
 */
	void flush(void);
/*
 * @Method:    freeTileMemory
 * @FullName:  vectorTileStore::freeTileMemory
 * @Access:    public 
 * @Returns:   void
 * @Parameter: char * tileValue
 * @purpose:	释放 vectorTileStore::getTile 中申请的内存空间 
 */
	void freeTileMemory( char* tileValue );
	VectorTileStore(void);
	~VectorTileStore(void);
};






#endif