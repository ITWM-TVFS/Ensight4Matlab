%% getTimesteps(object)
% Returns timestep vector
%
% INPUT
%  object    : |EnsightLib| object
%
% OUTPUT
%  timesteps : numeric
%
% USAGE
%  timesteps = object.getTimesteps
%

%%
function timesteps = getTimesteps(this)
  timesteps = this.timeSteps;
end