#include "utils.h"
#include <fc/crypto/sha256.hpp>
#include <queue>

using namespace std;
namespace fantasybit {

std::string hashit(const std::string &in) {
    return fc::sha256::hash(in).str();
}


// Three-way comparison function:
//   if a < b: negative result
//   if a > b: positive result
//   else: zero result
int Int32Comparator::Compare(const leveldb::Slice& a, const leveldb::Slice& b) const {
    int32_t ia = *(reinterpret_cast<const int32_t *>(a.data()));
    int32_t ib = *(reinterpret_cast<const int32_t *>(b.data()));

    //qDebug() << ia << ib << "yoyo";
    if (ia < ib) return -1;
    if (ia > ib) return +1;
    return 0;
}

// Ignore the following methods for now:
const char* Int32Comparator::Name() const { return "Int32Comparator"; }
void Int32Comparator::FindShortestSeparator(std::string*, const leveldb::Slice&) const { }
void Int32Comparator::FindShortSuccessor(std::string*) const { }

std::string makeMerkleRoot(decltype(fantasybit::MerkleTree::default_instance().leaves()) &in) {
    std::queue<fc::sha256> merkle;

    for (const auto elem : in ) {
        fc::sha256 digest = fc::sha256::hash(elem);

        merkle.push(digest);
    }

    if (merkle.empty())
        return "";//(fc::sha256) NULL;

    int rows = 0;
    while (merkle.size() > 1) {
       /*
        if (merkle.size() % 2 != 0) {
            merkle.push(merkle.back());
        }
       */
        queue<fc::sha256> new_merkle;

        rows++;
        int j =  merkle.size();

        while (merkle.size() > 1) {
            j =  merkle.size();

            fc::sha256 first = merkle.front();
            merkle.pop();

            j =  merkle.size();

            fc::sha256 second = merkle.front();
            merkle.pop();

            j =  merkle.size();

            string concat;

            concat = (string) first + (string) second;

            new_merkle.push(fc::sha256::hash(concat));
        }
        if ( merkle.size() == 1) {
            new_merkle.push(merkle.front());
            merkle.pop();
        }

        j =  merkle.size();

        merkle = new_merkle;
    }

    return merkle.front();

    return "";
}



/*
void displayBlock(leveldb::DB *db, int32_t blockNum) {
    // Print out block
    cout << "*** displaying block" << endl;

    // 381 block is first with valid timestamp
    int32_t num = blockNum;

    string value;
    leveldb::Slice snum((char*)&num, sizeof(int32_t));

    auto ret = db->Get(leveldb::ReadOptions(), snum, &value);
    if ( !ret.ok() ){
        cout << "block not found " << num << endl;
    }

    fantasybit::Block block{};
    block.ParseFromString(value);

    cout << "block: " << block.DebugString() << endl;
}

void displayHeaders(leveldb::DB *db) {
    leveldb::Iterator* it = db->NewIterator(leveldb::ReadOptions());
    for (it->SeekToFirst(); it->Valid(); it->Next()) {
        fantasybit::Block block{};
        block.ParseFromString(it->value().ToString());

        cout << "block: " << block.signedhead().DebugString() << endl;
    }

    if (!it->status().ok()) {
        cout << "Errors during scan" << endl;
    }

    delete it;
}

void displayTimestamps(leveldb::DB *db) {
    leveldb::Iterator* it = db->NewIterator(leveldb::ReadOptions());
    for (it->SeekToFirst(); it->Valid(); it->Next()) {
        fantasybit::Block block{};
        block.ParseFromString(it->value().ToString());

        int32_t block_num = block.signedhead().head().num();

        cout << "Timestamp " << block_num << ": " << block.signedhead().head().timestamp() << endl;
    }

    if (!it->status().ok()) {
        cout << "Errors during scan" << endl;
    }

    delete it;
}

void displayDiagnostics(leveldb::DB *db) {
    leveldb::Iterator* it = db->NewIterator(leveldb::ReadOptions());

    fantasybit::Block prevBlock{};

    // Get Genesis Block
    it->SeekToFirst();
    prevBlock.ParseFromString(it->value().ToString());
    it->Next();

    // Loop through rest of blocks
    for (; it->Valid(); it->Next()) {
        bool displayBlock = false;
        fantasybit::Block curBlock{};
        curBlock.ParseFromString(it->value().ToString());

        //cout << "prevBlockNum:" << prevBlock.signedhead().head().num() << endl;
        //cout << "curBlockNum:" << curBlock.signedhead().head().num() << endl;

        if (prevBlock.signedhead().head().timestamp() > curBlock.signedhead().head().timestamp()) {
            cout << "* Previous Block has greater timestamp for block number: " << curBlock.signedhead().head().num() << endl;
            displayBlock = true;
        }

        // ts for Jan 1 2015 = 1420099200
        if (curBlock.signedhead().head().timestamp() < 1420099200) {
            cout << "* Block has timestamp before Jan 1st 2015 at 12:00am" << endl;
            displayBlock = true;
        }

        if (curBlock.signedhead().head().transaction_id() != "") {
            cout << "* Block has a transaction_id that's not blank" << endl;
            displayBlock = true;
        }

        if (displayBlock) {
            cout << "* current block:" << curBlock.signedhead().DebugString() << endl;
        }

        //cout << "block: " << block.signedhead().head().timestamp() << endl;

        prevBlock = curBlock;
    }

    if (!it->status().ok()) {
        cout << "Errors during scan" << endl;
    }

    delete it;
}
*/
}
