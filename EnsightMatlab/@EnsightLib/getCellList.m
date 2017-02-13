%% getCellList(object, part, timestep)
% Counts number of cells per celltype
%
% INPUT
%  object   : |EnsightLib| object
%  part     : (int|string) |EnsightPart| identifier (if multiple parts exist)
%  timestep : (double) (transient objects only)
%
% OUTPUT
%  cList    : (8x2 Cell-Array) {string, int} number of cells per type
%
% USAGE
%  cList = object.getCellList(part, timestep)
%  cList = object.getCellList(part) (static)
%  cList = object.getCellList(timestep) (only one part)
%  cList = object.getCellList() (static, only one part)
%

%%
function CList = getCellList(varargin)

    assert(nargin >= 1 && nargin <= 3, 'EnsightLib::getCellList - Invalid number of input arguments. Type `help EnsightLib.getCellList` for further information');
  
    % Verify input
    if nargin == 3 % general case        
        this = varargin{1};
        part = this.verifyPartIdentifier(varargin{2});
        timestep = varargin{3};
    elseif nargin == 2
        this = varargin{1};
        assert(size(this.EnsightPartList,1)==1 || ~this.isTransient(),'EnsightLib::getCellList - Object is either transient or contains multiple parts. Type `help EnsighLib.getCellList` for further information');
        if(this.isTransient()) % transient object, only one part
            timestep = varargin{2};
            part = this.EnsightPartList{1,1};
        else % static object, possibly more than one part
            timestep = this.getTimesteps();
            part = this.verifyPartIdentifier(varargin{2});
        end
    elseif nargin == 1 % static, one-part object
        this = varargin{1};
        assert(size(this.EnsightPartList,1)==1 && ~this.isTransient(),'EnsightLib::getCellList - Object is either transient or contains multiple parts. Type `help EnsighLib.getCellList` for further information');
        
        part = this.EnsightPartList{1,1};
        timestep = this.getTimesteps();
    end
    
    assert(length(timestep)==1, 'EnsightLib::getCellList - Only one timstep at one time supported. Type `help EnsightLib.getCellList` for further information');
    assert(ismember(timestep,this.getTimesteps),'EnsightLib::getCellList - Invalid timestep. Type `object.getTimesteps` to see all existing timesteps or `help EnsightLib.getCellList` for further information');
    assert(part>=0,'EnsightLib::getCellList - Input2(part) is not valid. Type `help EnsighLib.getCellList` for further information');
        
    CList = cell(8,2);
    CList(:,1) = {'Point';'Bar';'Triangle';'Quadrangle';'Tetrahedron';'Pyramid';'Wedge';'Hexahedron'};
    
    valueArray = [3, 1, 5, 0, 0];
    cCount = EnsightLib_interface('part', 'getCellList', valueArray, this.getObjectHandle(), part, timestep);
 
    for i=1:8
        CList{i,2} = cCount(i);
    end
    
end