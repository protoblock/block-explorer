#include "blockchain.h"
#include <QDebug>

namespace fantasybit_bx {

Blockchain::Blockchain() {}

void Blockchain::init() {
    Int32Comparator *cmp = new Int32Comparator();
    leveldb::Options optionsInt;
    optionsInt.create_if_missing = false;
    optionsInt.error_if_exists = false;
    optionsInt.comparator = cmp;

    leveldb::Status status;

    leveldb::DB *db;

#ifdef JAYDESK
    status = leveldb::DB::Open(optionsInt, "D:/data/working-explorer/block/blockchain", &db);

#else
    status = leveldb::DB::Open(optionsInt, StaticUtil::DATAPATH+"/blockchain", &db);

#endif

    if (!status.ok()) {
        cout << "!!! error opening db block/blockchain" << endl;
        cout << "!!! " << status.ToString() << endl;
        exit(1);
    }

    /*
    int32_t num{3861};
    fantasybit_bx::Block b{};
    leveldb::Slice s((char*)&num, sizeof(int32_t));
    std::string value;
    auto ret = (*db)->Get(leveldb::ReadOptions(),s,&value);
    if ( !ret.ok() ){
        qWarning() << "block not found " << num;
    }
    else
        b.ParseFromString(value);
*/

    this->db = db;

    // Get Block Height
    leveldb::Iterator* iit = db->NewIterator(leveldb::ReadOptions());
    iit->SeekToLast();


    if (!iit->Valid()) {
        cout << "!!! Error reading iterator" << endl;
        exit(1);
    }

    fantasybit::Block block{};
    auto str = iit->value().ToString();
    block.ParseFromString(str);

    auto key = iit->key();
    auto value = iit->value();

    this->block_height = block.signedhead().head().num();

    //delete iit;
    // misc
    this->it = iit;
    it->SeekToFirst();
}

Blockchain::~Blockchain() {
    delete this->it;
    delete this->db;
}

int32_t Blockchain::GetBlockHeight() const {
    return this->block_height;
}

fantasybit::Block Blockchain::GetCurrentBlock() const {
    fantasybit::Block b{};
    b.ParseFromString(it->value().ToString());

    return b;
}

fantasybit::Block Blockchain::GetBlock(int32_t num) const {
    fantasybit::Block b{};
    leveldb::Slice s((char*)&num, sizeof(int32_t));
    std::string value;
    auto ret = db->Get(leveldb::ReadOptions(),s,&value);
    if ( !ret.ok() ){
        qWarning() << "block not found " << num;
    }
    else
        b.ParseFromString(value);

    return b;
}


void Blockchain::Seek(int32_t n) const {
    leveldb::Slice s((char*)&n, sizeof(int32_t));
    it->Seek(s);
    bool v = false;
    v = it->Valid();
    std::string ss = it->value().ToString();
}

void Blockchain::SeekToFirst() const {
    it->SeekToFirst();
}

void Blockchain::SeekToLast() const {
    it->SeekToLast();
}

void Blockchain::Next() const {
    it->Next();
}

void Blockchain::Prev() const {
    it->Prev();
}

bool Blockchain::Valid() const {
    return it->Valid();
}

bool Blockchain::Verify() const {
    return true;
}

void Blockchain::Test() {
    Blockchain bc{};

    cout << "*** Block Height: " << bc.GetBlockHeight() << endl;

    //bc.SeekToFirst();
    //cout << "*** Genesis Block: " << bc.GetCurrentBlock().DebugString() << endl;

    //bc.Seek(2);
    //cout << "Block 2: " << bc.GetCurrentBlock().DebugString() << endl;

    //bc.SeekToLast();
    //cout << "Block " << bc.GetBlockHeight() << ": " << bc.GetCurrentBlock().DebugString() << endl;

    // Print First 10 Blocks
    //bc.SeekToFirst();
    //for (int i = 0; i < 10 && bc.Valid(); ++i, bc.Next()) {
    //    fantasybit::Block b = bc.GetCurrentBlock();
    //    cout << "Block " << b.signedhead().head().num() << ": " << b.signedhead().DebugString() << endl;
    //}

    // Print Last 10 Blocks
    //bc.SeekToLast();
    //for (int i = 10; i > 0 && bc.Valid(); --i, bc.Prev()) {
    //    fantasybit::Block b = bc.GetCurrentBlock();
    //    cout << "Block " << b.signedhead().head().num() << ": " << b.signedhead().DebugString() << endl;
    //}
}

/*
void blockchain::test(leveldb::DB *db) {
    // 381 block is first with valid timestamp
    int32_t num = 1;

    string value;
    leveldb::Slice snum((char*)&num, sizeof(int32_t));

    auto ret = db->Get(leveldb::ReadOptions(), snum, &value);
    if ( !ret.ok() ){
        cout << "block not found " << num << endl;
    }

    fantasybit::Block block{};
    block.ParseFromString(value);

    fc::sha256 merkle = blockchain::create_merkle(block);

    //cout << "block: " << block.DebugString() << endl;
    cout << "trn_id: " << block.signedhead().head().transaction_id() << endl;
    cout << "merkle: " << (string) merkle << endl;
}

void blockchain::new_blockchain(leveldb::DB *src, leveldb::DB *dest) {
    leveldb::Iterator* it = src->NewIterator(leveldb::ReadOptions());
    for (it->SeekToFirst(); it->Valid(); it->Next()) {
        leveldb::Slice key = it->key();

        // construct block (add merkle)
        fantasybit::Block block{};
        block.ParseFromString(it->value().ToString());

        fantasybit::SignedBlockHeader signedhead{};
        signedhead.CopyFrom(block.signedhead());

        fantasybit::BlockHeader head{};
        head.CopyFrom(block.signedhead().head());

        fc::sha256 merkle = blockchain::create_merkle(block);
        head.set_transaction_id((string) merkle);

        signedhead.mutable_head()->CopyFrom(head);
        block.mutable_signedhead()->CopyFrom(signedhead);

        // Write to Dest db
        leveldb::WriteOptions write_options;
        write_options.sync = true;
        dest->Put(write_options, key, block.SerializeAsString());
    }
    assert(it->status().ok());  // Check for any errors found during the scan
    delete it;
}

bool blockchain::verify_block(fantasybit::Block block) {
    return true;
}

void blockchain::verify_blocks(leveldb::DB *db) {
    leveldb::Iterator* it = db->NewIterator(leveldb::ReadOptions());
    for (it->SeekToFirst(); it->Valid(); it->Next()) {
        leveldb::Slice key = it->key();

        fantasybit::Block block{};
        block.ParseFromString(it->value().ToString());

        if (!verify_block(block)) {
            cout << "!!! Invalid Block" << endl;
            cout << block.DebugString() << endl;
        }
    }
    assert(it->status().ok());  // Check for any errors found during the scan
    delete it;
}
*/
}
