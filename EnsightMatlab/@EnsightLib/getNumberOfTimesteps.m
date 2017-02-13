%% getNumberOfTimesteps(object)
% Returns the number of time steps 
%
% INPUT
%  object   : |EnsightLib| object
%
% OUTPUT
%  num      : (int) 
%
% USAGE
%  num  = object.getNumberOfTimesteps()
%

function num = getNumberOfTimesteps(this)
  num = length(this.timeSteps);
end