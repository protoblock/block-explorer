#include "blockchain.h"

#include <queue>

blockchain::blockchain()
{

}

fc::sha256 blockchain::create_merkle(fantasybit::Block block) {
    queue<fc::sha256> merkle;
    
    for (int i = 0; i < block.signed_transactions_size();  ++i) {
        // Transaction not signedtransaction?
        fantasybit::SignedTransaction st = block.signed_transactions(i);
        fc::sha256 digest = fc::sha256::hash(st.SerializeAsString());
        
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
    int32_t num = 1000;

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

        cout << block.DebugString() << endl;
    }
    assert(it->status().ok());  // Check for any errors found during the scan
    delete it;
}
