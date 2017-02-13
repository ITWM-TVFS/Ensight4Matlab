%% setTransient(object, timesteps)
% Set timesteps for a non-static object
%
% INPUT
%  object    : |EnsightObj| object
%  timesteps : (double) timestep-vector
%
% OUTPUT
%  none
%
% USAGE
%  object.setTransient(timesteps)
%

%%
function setTransient(this, values)
    % Check edit mode
    assert(this.inEditMode(),'EnsightObj::setTransient - Not in edit mode');
    
    valueArray = [2, 0, 5, 0];
    EnsightLib_interface('obj', 'setTransient', valueArray, this.getObjectHandle(), values);
    this.timeSteps = values;
end