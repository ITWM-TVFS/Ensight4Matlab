%% isTransient(object)
% returns (false) if object is static, (true) else 
%
% INPUT
%  object : |EnsightLib| object
%
% OUTPUT
%  result : (bool) true(multiple timesteps)/false(single timestep)
%
% USAGE
%  tra = object.isTransient
%

%%
function ok = isTransient(this)
  ok = length(this.timeSteps) > 1;
end