#include <iostream>
#include <sstream>
#include <string>
#include <fstream>
#include <map>
#include <list>

#include <GDAL/gdal.h>

#include <boost/filesystem.hpp>
#include <boost/program_options/options_description.hpp>
#include <boost/program_options/parsers.hpp>
#include <boost/program_options/variables_map.hpp>
#include <boost/graph/undirected_dfs.hpp>


#include <boost/foreach.hpp>
#include <boost/progress.hpp>


#include "Types.h"
#include "ReadInMap.h"
#include "PrintGraphsToFile.h"
#include "ReadGraphsFromFile.h"
#include "AssignIdThroughHydroFile.h"



int main(int argc, char **argv)
{
    
    
    /**********************************/
    /*        Program options         */
    /**********************************/
    // Need to specify elevation grid
    // Need to specify channel

    std::string fd_file;
    std::string hydro_paths_file;
    std::string channel_graph_file;

    
    namespace prog_opt = boost::program_options;
    prog_opt::options_description desc("Allowed options");
    desc.add_options()
    ("help,h", "produce help message")
    ("flow-dir-map,f", prog_opt::value<std::string>(&fd_file), "path of the gdal capatible flow direction data file")
    ("channel-graph,g", prog_opt::value<std::string>(&channel_graph_file), "path of the graphml representation of the channel")
    ("hydro-paths-file,p", prog_opt::value<std::string>(&hydro_paths_file)->default_value("hydro-paths.tif"), "path of the output map where each pixel is assigned the location on channel that the pixel is hydrologically connected to ");
    
    
    
    prog_opt::variables_map vm;
    prog_opt::store(prog_opt::parse_command_line(argc, argv, desc), vm);
    prog_opt::notify(vm);
    if (vm.count("help"))
    {
        std::cout << desc << "\n";
        return 1;
    }

    boost::filesystem::path fd_file_path(fd_file);
    boost::filesystem::path hydro_paths_file_path(hydro_paths_file);
    boost::filesystem::path channel_graph_path(channel_graph_file);
    
    

    // Check file exists
    if (!fs::exists(fd_file_path))
    {
        std::stringstream ss;
        ss << fd_file_path << " does not exist";
        throw std::runtime_error(ss.str());
        return (EXIT_FAILURE);
    }
    if (!fs::exists(channel_graph_path))
    {
        std::stringstream ss;
        ss << channel_graph_path << " does not exist";
        throw std::runtime_error(ss.str());
        return (EXIT_FAILURE);
    }
    
    /**********************************/
    /*       Create graph object      */
    /**********************************/
    Graph channel_grph;
    
    
    /**********************************/
    /*         Read in Graph           */
    /**********************************/
    std::cout << "\n\n*************************************\n";
    std::cout <<     "*             Read in Graphs          *\n";
    std::cout <<     "*************************************" << std::endl;
    //    readGraphFromFile(control_graph_path, control_grph);
    readGraphFromFile(channel_graph_path, channel_grph);
    
    
    /**********************************/
    /*    Read in flow direction raster    */
    /**********************************/
    std::cout << "\n\n*************************************\n";
    std::cout <<     "*      Reading in flow direction maps     *\n";
    std::cout <<     "*************************************" << std::endl;
    // Third the flow direction data
    std::tuple<Map_Int_SPtr, std::string, GeoTransform> gdal_fd_map = read_in_map<int32_t>(fd_file_path, GDT_Int32, NO_CATEGORISATION);
    Map_Int_SPtr fd_map(std::get<0>(gdal_fd_map));
    std::string & fdWKTprojection(std::get<1>(gdal_fd_map));
    GeoTransform & fdTransform(std::get<2>(gdal_fd_map));
    
    
//    //Check maps for consistency (same dimensions)
//    if (feature_map->NCols() != fd_map->NCols())
//    {
//        throw std::runtime_error("Number of columns in the two comparison maps non-identical");
//    }
//    
//    if (feature_map->NRows() != fd_map->NRows())
//    {
//        throw std::runtime_error("Number of rows in the two comparison maps non-identical");
//    }
    
    std::map<int, std::map<int, VertexDescriptor>  > channel_pixels;
    std::pair<VertexIterator, VertexIterator> vp;
    for (vp = boost::vertices(channel_grph); vp.first != vp.second; ++vp.first)
    {
        int i = channel_grph[*vp.first].row;
        int j = channel_grph[*vp.first].col;
        VertexDescriptor v = *vp.first;
        channel_pixels[i].insert(std::make_pair(j, v));
    }

    
    
    
    /*******************************************************************/
    /*     Assign pixels to hydrologically connected channel pixel     */
    /*******************************************************************/
    std::cout << "\n\n*************************************\n";
    std::cout << "*  Assign pixels to hydrologically connected channel pixel    *\n";
    std::cout << "*************************************" << std::endl;
    
    
    //    int32_t no_changes_val = -9;
    //    Map_Int_SPtr creek_id_map(new Map_Int(dem_map->NRows(), dem_map->NCols(), no_changes_val));
    //    creek_id_map->SetNoDataValue(no_changes_val);
    
    int32_t no_connection = -9;
    Map_Int_SPtr hydro_connect_map(new Map_Int(fd_map->NRows(), fd_map->NCols(), no_connection));
    hydro_connect_map->SetNoDataValue(no_connection);
    
    typedef std::pair<int, int> Position;
    std::list<Position> channel_pixels_list;
    for (vp = boost::vertices(channel_grph); vp.first != vp.second; ++vp.first)
    {
        int row = channel_grph[*vp.first].row;
        int col = channel_grph[*vp.first].col;
        channel_pixels_list.push_back(Position(row, col));
    }
    
    
    /*******************************************************************/
    /*     Assign pixels to hydrologically connected channel pixel     */
    /*******************************************************************/
    std::cout << "\n\n*************************************\n";
    std::cout << "*  Assign pixels to hydrologically connected channel pixel    *\n";
    std::cout << "*************************************" << std::endl;
    
    boost::progress_display show_progress_hccp(channel_pixels_list.size());
    while (!channel_pixels_list.empty())
    {
        
        //typedef std::pair<const double, Position> ChannelLocationType;
        std::list<Position>::iterator lowest_loc_it;
        double lowest_val = 9999999.99;
        for (std::list<Position>::iterator loc_it = channel_pixels_list.begin(); loc_it != channel_pixels_list.end(); ++loc_it)
        {
            double val = channel_grph[channel_pixels[loc_it->first][loc_it->second]].elevation;
            if (val < lowest_val)
            {
                lowest_loc_it = loc_it;
                lowest_val = val;
            }
        }
        
        
        
        assignIdThroughHydroPath(channel_grph[channel_pixels[lowest_loc_it->first][lowest_loc_it->second]], hydro_connect_map, fd_map, channel_pixels);
        channel_pixels_list.erase(lowest_loc_it);
        ++show_progress_hccp;
    }

    
//    std::map<double, VertexDescriptor> elevation_map;
//    for (vp = boost::vertices(channel_grph); vp.first != vp.second; ++vp.first)
//    {
//        elevation_map.insert(std::make_pair(channel_grph[*vp.first].elevation, *vp.first));
//    }
//    
//    
//    boost::progress_display show_progress_hccp(elevation_map.size());
//    typedef std::pair<const double, VertexDescriptor> ElevationMapPair;
//    BOOST_FOREACH(ElevationMapPair & node, elevation_map)
//    {
//        //std::cout << channel_grph[node.second].elevation << "\n";
//        assignIdThroughHydroPath(channel_grph[node.second], hydro_connect_map, fd_map, channel_pixels);
//        ++show_progress_hccp;
//    }
    
    
    
    
    /********************************************/
    /* Print resultent DEM					    */
    /********************************************/
    std::cout << "\n\n*************************************\n";
    std::cout <<     "*  Saving hydro connectivity map    *\n";
    std::cout <<     "*************************************" << std::endl;
    std::string driverName = "GTiff";
    write_map(hydro_paths_file, GDT_Int32, hydro_connect_map, fdWKTprojection, fdTransform, driverName);
    //    write_map(channel_id_file, GDT_Int32, creek_id_map, fdWKTprojection, fdTransform, driverName);
    
    
    
   
    
    return (EXIT_SUCCESS);

    
}

