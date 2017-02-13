%% clean(object, part, timestep)
% Erases all data at 'timestep'
%
% INPUT
%  object   : |EnsightLib| object
%  part     : |EnsightPart| identifier (int or string)
%  timestep : (integer) 
%
% OUTPUT
%  none
%
% USAGE
%  object.clean(timestep)
%  object.clean(part, timestep)
%

%%
function clean(varargin)
    
    valueArray = [2 0 5 0];
    if nargin == 3
        this = varargin{1};
        part_id = this.verifyPartIdentifier(varargin{2});
        timestep = varargin{3};
        
        % Check input and edit mode
        assert(isa(this,'EnsightLib'),'EnsightLib::clean - Input1(object) is not valid. Type `help EnsighLib.clean` for detailed information');
        assert(isa(part_id,'char') || isnumeric(part_id),'EnsightLib::clean - Input2(part_id) is not valid. Type `help EnsighLib.clean` for detailed information');
        assert(isnumeric(timestep)','EnsightLib::clean - Input3(timestep) is not valid. Type `help EnsighLib.clean` for detailed information');
        assert(this.inEditMode(),'EnsightLib::clean - Not in edit mode');
    
        % Call |EnsightPart| method
        EnsightLib_interface('part_clean',this.getObjectHandle(), part_id, timestep);
    elseif nargin == 2
        this = varargin{1};
        timestep = varargin{2};
        
        % Check input and edit mode
        assert(isa(this,'EnsightLib'),'EnsightLib::clean - Input1(object) is not valid. Type `help EnsighLib.clean` for detailed information');
        assert(isnumeric(timestep)','EnsightLib::clean - Input2(timestep) is not valid. Type `help EnsighLib.clean` for detailed information');
        assert(this.inEditMode(),'EnsightLib::clean - Not in edit mode');
    
        % Call |EnsightPart| method
        EnsightLib_interface('obj', 'clean', valueArray, this.getObjectHandle(), timestep);
    else
        error('EnsightLib::clean - Invalid number of input arguments. Type `help EnsightLib.clean` for detailed information');
    end
end