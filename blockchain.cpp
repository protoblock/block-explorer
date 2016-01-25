#include "blockchain.h"

#include <queue>

blockchain::blockchain()
{
    cout << "Constructor called" << endl;

    Int32Comparator *cmp = new Int32Comparator();
    leveldb::Options optionsInt;
    optionsInt.create_if_missing = true;
    optionsInt.error_if_exists = false;
    optionsInt.comparator = cmp;

    leveldb::Status status;

    leveldb::DB *db;
    status = leveldb::DB::Open(optionsInt, "/Users/satoshi/ryan_work/block/blockchain", &db);
    if (!status.ok()) {
        cout << "!!! error opening db block/blockchain" << endl;
        cout << "!!! " << status.ToString() << endl;
        exit(1);
    }

    this->db = db;

    // Get Block Height
    leveldb::Iterator* it = db->NewIterator(leveldb::ReadOptions());
    it->SeekToLast();

    if (!it->Valid()) {
        cout << "!!! Error reading iterator" << endl;
        exit(1);
    }

    fantasybit::Block block{};
    block.ParseFromString(it->value().ToString());

    this->block_height = block.signedhead().head().num();

    // misc
    this->it = it;
    it->SeekToFirst();
}

blockchain::~blockchain() {
    cout << "Deconstructor called" << endl;
    delete this->db;
    delete this->it;
}

int32_t blockchain::GetBlockHeight() {
    return this->block_height;
}

fantasybit::Block blockchain::GetCurrentBlock() {
    fantasybit::Block b{};
    b.ParseFromString(it->value().ToString());

    return b;
}

void blockchain::Seek(int32_t n) {
    leveldb::Slice s((char*)&n, sizeof(int32_t));
    it->Seek(s);
}

void blockchain::SeekToFirst() {
    it->SeekToFirst();
}

void blockchain::SeekToLast() {
    it->SeekToLast();
}

void blockchain::Next() {
    it->Next();
}

void blockchain::Prev() {
    it->Prev();
}

bool blockchain::Valid() {
    return it->Valid();
}

bool blockchain::Verify() {
    return true;
}


/*
fantasybit::Block blockchain::getBlock(int32_t n) {
    fantasybit::Block b{};
    string value;
    leveldb::Slice snum((char*)&n, sizeof(int32_t));

    auto ret = this->db->Get(leveldb::ReadOptions(), snum, &value);
    if ( !ret.ok() ){
        cout << "block not found " << n << endl;
        exit(1);
    }

    b.ParseFromString(value);

    return b;
}
*/

/*
fc::sha256 blockchain::create_merkle(fantasybit::Block block) {
    queue<fc::sha256> merkle;
    
    for (int i = 0; i < block.signed_transactions_size();  ++i) {
        // Transaction not signedtransaction?
        //fantasybit::SignedTransaction st = block.signed_transactions(i);
        //fc::sha256 digest = fc::sha256::hash(st.SerializeAsString());

        fantasybit::Transaction trans = block.signed_transactions(i).trans();
        fc::sha256 digest = fc::sha256::hash(trans.SerializeAsString());
        
        merkle.push(digest);
    }

    if (merkle.empty())
        return (fc::sha256) NULL;

    while (merkle.size() > 1) {
        if (merkle.size() % 2 != 0) {
            merkle.push(merkle.back());
        }

        queue<fc::sha256> new_merkle;

        for (int i = 0; i < merkle.size(); i += 2) {
            fc::sha256 first = merkle.front();
            merkle.pop();

            fc::sha256 second = merkle.front();
            merkle.pop();

            string concat;

            concat = (string) first + (string) second;

            new_merkle.push(fc::sha256::hash(concat));
        }

        merkle = new_merkle;
    }

    return merkle.front();
}

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
