%% getVariableBounds(object, name, timestep)
% Returns upper/lower bound of the variable 'name'
%
% INPUT
%  object   : |EnsightLib| object
%  name     : (string) variable name
%  timestep : (integer) timestep (for transient objects only)
%
% OUTPUT
%  bound            : (numeric) Bounds of the variable 'name' at 'timestep'
%  (dimensions may depend on variable_type and variable_dimensions)
%  magnitude_bounds : (numeric) (optional) min/max magnitude. 3D-Variables only
%
% USAGE
%  bounds             = object.getVariableBound(name)
%  bounds             = object.getVariableBound(name,timestep)
%  [bounds,magnitude] = object.getVariableBound(name)
%  [bounds,magnitude] = object.getVariableBound(name,timestep)
%

%%
function varargout = getVariableBounds(this, name, timestep)
    assert(this.hasVariable(name),'EnsightObj::getVariableBounds - Variable does not exist');
   
    if nargin < 3
        assert(~this.isTransient(),'This is a transient object. A timestep needs to be specified');
        timestep = 0;
    end
    
    valueArray = [3, 1, 5, 1, 0];
    bounds = EnsightLib_interface('obj', 'getVariableBounds', valueArray, this.getObjectHandle(), name, timestep);
    
    if(size(bounds,1)==4)
        varargout{1} = bounds(1:3,:);
        if nargout==2
            varargout{2} = bounds(4,:);
        end
    else
        varargout{1} = bounds;
    end
end