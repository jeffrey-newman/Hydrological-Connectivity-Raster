#include "AssignIdThroughHydroFile.h"
#include <stack>


typedef std::pair<int, int> Position;

bool isContributer(int val, int fd_id)
{

	if (val >= 128)
	{
		if (fd_id == 128)
		{
			return (true);
		}
		// Then flow path goes to upper right   --- (-1, 1)
		val -= 128;
	}
	if (val >= 64)
	{
		if (fd_id == 64)
		{
			return (true);
		}
		// Then flow path goes upwards ---- (-1, 0)
		val -= 64;
	}
	if (val >= 32)
	{
		if (fd_id == 32)
		{
			return (true);
		}
		// Then flow path goes upper left --- (-1, -1)
		val -= 32;
	}
	if (val >= 16)
	{
		if (fd_id == 16)
		{
			return (true);
		}
		// Then flow path goes left --- (0, -1) 
		val -= 16;
	}
	if (val >= 8)
	{
		if (fd_id == 8)
		{
			return (true);
		}
		// then flow path goes lower left (1, -1)
		val -= 8;
	}
	if (val >= 4)
	{
		if (fd_id == 4)
		{
			return (true);
		}
		// then flow path goes down (1, 0)
		val -= 4;
	}
	if (val >= 2)
	{
		if (fd_id == 2)
		{
			return (true);
		}
		// then flow path goes lower right --- (1, 1)
		val -= 2;
	}
	if (val >= 1)
	{
		if (fd_id == 1)
		{
			return (true);
		}
		// then flow path goes right ---- (0, 1)
		val -= 1;
	}
	if (val != 0)
	{
		std::cout << "ERROR" << std::endl;
	}
	return (false);
}


bool isNotCreek(int row, int col, std::map<int, std::map<int, VertexDescriptor>  > &  channel_pixels)
{
    typedef std::map<int, std::map<int, VertexDescriptor>  >::iterator RowIt;
    typedef std::map<int, VertexDescriptor>::iterator ColIt;
    RowIt r_it = channel_pixels.find(row);
    if (r_it != channel_pixels.end())
    {
        std::map<int, VertexDescriptor> col_map = r_it->second;
        ColIt c_it = col_map.find(col);
        if (c_it != col_map.end())
        {
            return (false);
        }
        else
        {
            return (true);
        }
    }
    return (true);
}

bool isInMap(Position & loc, Map_Int_SPtr _fd_map)
{
	if (loc.first < _fd_map->NRows() && loc.second < _fd_map->NCols()) return (true);
	return (false);
}

void assignIdThroughHydroPath(ChannelNode & node, Map_Int_SPtr output_map, Map_Int_SPtr fd_map,  std::map<int, std::map<int, VertexDescriptor>  > &  channel_pixels)
{

	std::stack<Position> trailcrumbs;
	trailcrumbs.push(Position(node.row, node.col));


	do
	{
		/*****************************************************
		*          Get top most Position from stack          *
		*****************************************************/
		Position loc = trailcrumbs.top();
//		size_t size = trailcrumbs.size();
		trailcrumbs.pop();

		/*****************************************************
		*          Assign Position with stream pixel id      *
		*****************************************************/
		output_map->Get(loc.first, loc.second) = node.node_id;
		
		/*****************************************************************************************
		*          Search neighbourhood for contributing pixels, and add these to the stack      *
		******************************************************************************************/
		
		Position neighbour = Position(loc.first - 1, loc.second + 1);
		int val = 0;
		if (isInMap(neighbour, fd_map))
		{
			val = fd_map->Get(neighbour.first, neighbour.second);
			if (isContributer(val, 8))
			{
				// Need to Ensure is not a creek pixel;
				if (isNotCreek(neighbour.first, neighbour.second, channel_pixels) && (output_map->Get(neighbour.first, neighbour.second) != node.node_id))
				{
					// Is contributing pixel;
					trailcrumbs.push(neighbour);
				}

			}
		}
		neighbour = Position(loc.first - 1, loc.second);
		if (isInMap(neighbour, fd_map))
		{
			val = fd_map->Get(neighbour.first, neighbour.second);
			if (isContributer(val, 4))
			{
				// Need to Ensure is not a creek pixel;
				if (isNotCreek(neighbour.first, neighbour.second, channel_pixels) && (output_map->Get(neighbour.first, neighbour.second) != node.node_id))
				{
					// Is contributing pixel;
					trailcrumbs.push(neighbour);
				}
			}
		}
		neighbour = Position(loc.first - 1, loc.second - 1);
		if (isInMap(neighbour, fd_map))
		{
			val = fd_map->Get(neighbour.first, neighbour.second);
			if (isContributer(val, 2))
			{
				// Need to Ensure is not a creek pixel;
				if (isNotCreek(neighbour.first, neighbour.second, channel_pixels) && (output_map->Get(neighbour.first, neighbour.second) != node.node_id))
				{
					// Is contributing pixel;
					trailcrumbs.push(neighbour);
				}
			}
		}
		neighbour = Position(loc.first, loc.second - 1);
		if (isInMap(neighbour, fd_map))
		{
			val = fd_map->Get(neighbour.first, neighbour.second);
			if (isContributer(val, 1))
			{
				// Need to Ensure is not a creek pixel;
				if (isNotCreek(neighbour.first, neighbour.second, channel_pixels) && (output_map->Get(neighbour.first, neighbour.second) != node.node_id))
				{
					// Is contributing pixel;
					trailcrumbs.push(neighbour);
				}
			}
		}
		neighbour = Position(loc.first + 1, loc.second - 1);
		if (isInMap(neighbour, fd_map))
		{
			val = fd_map->Get(neighbour.first, neighbour.second);
			if (isContributer(val, 128))
			{
				// Need to Ensure is not a creek pixel;
				if (isNotCreek(neighbour.first, neighbour.second, channel_pixels) && (output_map->Get(neighbour.first, neighbour.second) != node.node_id))
				{
					// Is contributing pixel;
					trailcrumbs.push(neighbour);
				}
			}
		}
		neighbour = Position(loc.first + 1, loc.second);
		if (isInMap(neighbour, fd_map))
		{
			val = fd_map->Get(neighbour.first, neighbour.second);
			if (isContributer(val, 64))
			{
				// Need to Ensure is not a creek pixel;
				if (isNotCreek(neighbour.first, neighbour.second, channel_pixels) && (output_map->Get(neighbour.first, neighbour.second) != node.node_id))
				{
					// Is contributing pixel;
					trailcrumbs.push(neighbour);
				}
			}
		}
		neighbour = Position(loc.first + 1, loc.second + 1);
		if (isInMap(neighbour, fd_map))
		{
			val = fd_map->Get(neighbour.first, neighbour.second);
			if (isContributer(val, 32))
			{
				// Need to Ensure is not a creek pixel;
				if (isNotCreek(neighbour.first, neighbour.second, channel_pixels) && (output_map->Get(neighbour.first, neighbour.second) != node.node_id))
				{
					// Is contributing pixel;
					trailcrumbs.push(neighbour);
				}
			}
		}
		neighbour = Position(loc.first, loc.second + 1);
		if (isInMap(neighbour, fd_map))
		{
			val = fd_map->Get(neighbour.first, neighbour.second);
			if (isContributer(val, 16))
			{
				// Need to Ensure is not a creek pixel;
				if (isNotCreek(neighbour.first, neighbour.second, channel_pixels) && (output_map->Get(neighbour.first, neighbour.second) != node.node_id))
				{
					// Is contributing pixel;
					trailcrumbs.push(neighbour);
				}
			}
		}


	} while (!trailcrumbs.empty());




}




