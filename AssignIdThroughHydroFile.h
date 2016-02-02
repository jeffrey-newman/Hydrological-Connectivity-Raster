#ifndef ASSIGN_ID_THROUGH_HYDRO_PATH
#define ASSIGN_ID_THROUGH_HYDRO_PATH

#include "Types.h"

void assignIdThroughHydroPath(ChannelNode & node, Map_Int_SPtr output_map, Map_Int_SPtr fd_map, std::map<int, std::map<int, VertexDescriptor>  > & channel_pixels);


#endif //ASSIGN_ID_THROUGH_HYDRO_PATH