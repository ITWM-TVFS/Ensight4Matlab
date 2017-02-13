%% setStatic(object)
% Delete all timesteps; Insert single timestep '0'
%
% INPUT
%  object : |EnsightLib| object
%
% OUTPUT
%  none
%
% USAGE
%  object.setStatic
%

%%
function setStatic(this)
    % Check edit mode
    assert(this.inEditMode(),'EnsightObj::setStatic - Not in edit mode');
    
    valueArray = [1, 0, 5];
    EnsightLib_interface('obj', 'setStatic', valueArray, this.getObjectHandle());
    this.timeSteps = 0;
end