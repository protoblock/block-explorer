#ifndef UTILS_H
#define UTILS_H

#include <iostream>
#include <leveldb/db.h>
#include <leveldb/comparator.h>
#include <proto/ProtoData.pb.h>

using namespace std;

class Int32Comparator : public leveldb::Comparator {
 public:
  int Compare(const leveldb::Slice& a, const leveldb::Slice& b) const;

  // Ignore the following methods for now:
  const char* Name() const;
  void FindShortestSeparator(std::string*, const leveldb::Slice&) const;
  void FindShortSuccessor(std::string*) const;
};

/*
void displayBlock(leveldb::DB *db, int32_t blockNum);
void displayHeaders(leveldb::DB *db);
void displayTimestamps(leveldb::DB *db);
void displayDiagnostics(leveldb::DB *db);
*/


#endif // UTILS_H
