%% getVariable(object, part, name, values, vartype, timestep)
% get variable values
%
% INPUT 
%  object    : |EnsightObj| object
%  part      : (int||string) part_idetifier 
%  name      : (string) variable name
%  timestep  : (integer) Not needed if the object is static
%
% OUTPUT
%  variable  : (numeric) dimensions depend on variable_type
%  magnitude : (numeric,optional) 3D-variables only
%
% USAGE
%  var             = object.getVariable(part, name)
%  var             = object.getVariable(part, name, timestep)
%  [var,magnitude] = object.getVariable(part, name)
%  [var,magnitude] = object.getVariable(part, name, timestep)
%

%%
function varargout = getVariable(this, part, name, timestep)
    assert(this.hasVariable(name),'EnsightLib::getVariable - Variable does not exist');
   
    part = this.verifyPartIdentifier(part);
    assert(part>-1,'EnsightLib::getVariable - Invalid part_id');
    if nargin < 4
        assert(~this.isTransient(),'This is a transient object. A timestep needs to be specified');
        timestep = 0;
    end
    
    valueArray = [4, 1, 5, 0, 1, 0];
    VAR = EnsightLib_interface('part', 'getVariableValues', valueArray, this.getObjectHandle(), part, name, timestep);
    
    
    if(size(VAR,1)==4)
        varargout{1} = VAR(1:3,:);
        if nargout==2
            varargout{2} = VAR(4,:);
        end
    else
        varargout{1} = VAR;
    end
    
end