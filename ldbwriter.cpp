#include "ldbwriter.h"

using namespace std;
namespace fantasybit {

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
