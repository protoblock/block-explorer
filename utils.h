#ifndef UTILS_H
#define UTILS_H

#include <iostream>
#include <leveldb/db.h>
#include <leveldb/comparator.h>
#include <proto/ProtoData.pb.h>
#include <proto/StateData.pb.h>
#include <fc/crypto/sha256.hpp>
#include <string>

//using namespace std;
namespace fantasybit_bx {

class Int32Comparator : public leveldb::Comparator {
 public:
  int Compare(const leveldb::Slice& a, const leveldb::Slice& b) const;

  // Ignore the following methods for now:
  const char* Name() const;
  void FindShortestSeparator(std::string*, const leveldb::Slice&) const;
  void FindShortSuccessor(std::string*) const;

};

struct StaticUtil {
    static std::string DATAPATH;
};
std::string makeMerkleRoot(decltype(fantasybit::MerkleTree::default_instance().leaves()) &in);
std::string hashit(const std::string &in);
std::string hashit(const google::protobuf::Message  &in);


}

//decltype(MerkleTree:: ::default_instance().data()) in,



/*
void displayBlock(leveldb::DB *db, int32_t blockNum);
void displayHeaders(leveldb::DB *db);
void displayTimestamps(leveldb::DB *db);
void displayDiagnostics(leveldb::DB *db);
*/


#endif // UTILS_H
