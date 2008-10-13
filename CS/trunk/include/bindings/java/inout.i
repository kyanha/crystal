%apply unsigned long long { uint64 };
%apply long long { int64 };
%apply long { uint32, uint, int32, int, size_t };
%apply int { uint16, int16 };
%apply short { uint8 };
%apply signed char { int8 };

%apply signed char * OUTPUT { int8 & v };
%apply short * OUTPUT { uint8 & v };
%apply int * OUTPUT { int16 & v, uint16 & v };
%apply long * OUTPUT { int32 & v, int & v, uint32 & v, uint & v };
%apply long long * OUTPUT { int64 & v };
%apply unsigned long long * OUTPUT { uint64 & v };
%apply bool * OUTPUT { bool & v, bool & coplanar };
%apply float * OUTPUT { float & v, float & valid_radius };
%apply double * OUTPUT { double & v };
%apply csRef<iEvent> & OUTPUT { csRef<iEvent> & v };
%apply csRef<iBase> & OUTPUT { csRef<iBase> & v };
%apply char * OUTPUT { char *& v };
%apply (void *&, size_t &) { (const void *& v, size_t & size) };
%apply (void *,size_t) { (const void *v, size_t size) };
%apply (csTriangleMeshEdge *,size_t) { (csTriangleMeshEdge* edges, size_t num_edges) };
%apply (csPlane3 *,size_t) { (csPlane3* planes, size_t num_vertices) };
