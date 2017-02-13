%% search(object, lineSegments)
% Returns the vertices of an |EnsightPart| for multiple timesteps
%
% INPUT
%  object       : |EnsightLib| object
%  lineSegments : (numeric, 3xN matrix) A set of 3-dimensional points
%  options      : (string, optional) 'c' for centroid, 'v' for volume,'vc'for both
% OUTPUT
%  cellList     : (int, 3xN) set of triplets [cell_id;celltype_id;part_id]
%                 If the search was not successful, the corresponding triplet is set to [-1;-1;-1]
%
%                 (int, 4xN), (int, 6xN) or (int, 7xN) depending on options
% USAGE
%  cellList     = object.search(lineSegments)
%  cellList     = object.search(lineSegments,options)

%%
function cellList = search(this, lineSegments, options)
    assert(nargin==2 || nargin==3,'EnsightLib::search - Invalid input. Call `help EnsightLib.search` for further information.');
    assert(size(lineSegments,1)==3,'EnsightLib::search - Invalid input (wrong dimensions). Call `help EnsightLib.search` for further information.');
    assert(size(lineSegments,2)>0,'EnsightLib::search - Invalid input (wrong dimensions). Call `help EnsightLib.search` for further information.');
    
    if(nargin == 2 )
        options = '0';
    else
        options = [num2str(length(options)),lower(options)];
    end
    
    valueArray = [3, 1, 5, 0, 1];
    buffer = EnsightLib_interface('obj', 'findCell', valueArray, this.getObjectHandle(), lineSegments, options);
    cellList = buffer;
end