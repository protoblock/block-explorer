#include "actions.h"

Actions::Actions()
{

}

fc::sha256 Actions::CreateMerkle(fantasybit::Block block) {
    queue<fc::sha256> merkle;

    for (int i = 0; i < block.signed_transactions_size();  ++i) {
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

void Actions::Test() {
    Blockchain bc{};
    bc.SeekToFirst();
    cout << "*** Genesis Block Merkle: " << bc.GetCurrentBlock().signedhead().head().transaction_id() << endl;
    cout << "*** CreateMerkle: " << (string) Actions::CreateMerkle(bc.GetCurrentBlock()) << endl;
}
