/*
 * About:
 */
#include <iostream>
#include <vector>
#include <string>

#include "xmeta_obj_generated.h"

using namespace std;

int main(int argc, char* argv[]) {
	flatbuffers::FlatBufferBuilder builder;

	// some ACL experiment
	std::string random_acl = "some random ACL entry";
	std::vector<flatbuffers::Offset<flatbuffers::String>> l_acl;
	l_acl.push_back(builder.CreateString(random_acl));
	auto acl = builder.CreateVector(l_acl);

	// some user attributes experiment
	auto ua = Createuser_attr(builder, builder.CreateString("name"),
	    builder.CreateString("lhotse"));
	std::vector<flatbuffers::Offset<user_attr>> l_user_attrs;
	l_user_attrs.push_back(ua);
	auto user_attrs = builder.CreateVector(l_user_attrs);
	
	// some sparse map experiment
	auto se = Createsparse_entry(builder, (uint64_t)0, (uint64_t)300);
	std::vector<flatbuffers::Offset<sparse_entry>> l_sparse_map;
	l_sparse_map.push_back(se);
	auto sparse_map = builder.CreateVector(l_sparse_map);

	auto xmeta = Createxmeta_format(builder, acl, user_attrs, sparse_map);

	builder.Finish(xmeta);

	// send builder over the network. Following code executes on target
	
	auto xmeta_rcv = Getxmeta_format(builder.GetBufferPointer());

	// verify ACL
	auto r_acl = xmeta_rcv->acl();
	if (r_acl->size() == 1 && r_acl->Get(0)->str() == random_acl) {
		std::cout << "test passed, " << r_acl->Get(0)->str() <<
		     std::endl;
	} else {
		std::cout << "test failed, received " << r_acl->Get(0)->str()
		    << std::endl;
	}

	// verify sparse map
	auto r_smap = xmeta_rcv->sparse_map();
	std::cout << "offset:" << r_smap->Get(0)->offset() << ", length:" <<
	    r_smap->Get(0)->length() << std::endl;

	// view user attrs
	auto r_uattr = xmeta_rcv->user_attrs();
	std::cout << "key:" << r_uattr->Get(0)->key()->str() << ", value:" << 
	    r_uattr->Get(0)->value()->str() << std::endl; 

	return 0;
}
