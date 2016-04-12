#include "ldbwriter.h"

using namespace std;
namespace fantasybit {

void fantasybit::LdbWriter::init() {
    //qDebug() << BlockMeta::descriptor()->file()->DebugString().data();
    write_sync.sync = true;

    leveldb::Options opt;
    opt.create_if_missing = true;
    opt.error_if_exists = false;

    leveldb::Status status;

    leveldb::DB *db;
    status = leveldb::DB::Open(opt, "D:/data/working-explorer/block/meta", &db);
    if (!status.ok()) {
        std::cout << "!!! error opening db block/meta" << std::endl;
        std::cout << "!!! " << status.ToString() << std::endl;
        exit(1);
    }

    this->db = db;
}

std::string LdbWriter::read(const std::string &id) {
    string value;
    db->Get(leveldb::ReadOptions(),id,&value);
    return value;
}

string LdbWriter::read(const string &id, google::protobuf::Message &msg) {
    auto str = read(id);
    msg.ParseFromString(str);
    return str;
}

}
