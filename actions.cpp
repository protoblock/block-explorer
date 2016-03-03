#include "actions.h"
#include "proto/StateData.pb.h"
#include "utils.h"

using namespace fantasybit;
Actions::Actions()
{

}

MerkleTree Actions::CreateMerkle(fantasybit::Block &block) {
    MerkleTree merkle;

    for (int i = 0; i < block.signed_transactions_size();  ++i) {
        fantasybit::Transaction trans = block.signed_transactions(i).trans();
        fc::sha256 digest = fc::sha256::hash(trans.SerializeAsString());

        *(merkle.add_leaves()) = digest.str();
    }

    merkle.set_root(makeMerkleRoot(merkle.leaves()));

    return merkle;

}

void Actions::Test() {
    Blockchain bc{};
    bc.SeekToFirst();
    cout << "*** Genesis Block Merkle: " << bc.GetCurrentBlock().signedhead().head().transaction_id() << endl;
    cout << "*** CreateMerkle: " << Actions::CreateMerkle(bc.GetCurrentBlock()).DebugString() << endl;
}
