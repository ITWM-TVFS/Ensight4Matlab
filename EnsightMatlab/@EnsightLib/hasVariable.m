%% hasVariable(object, name)
% Checks, whether a variable 'name' exists
%
% INPUT
%  object : |EnsightObj| object
%  name   : variable name (string)
%
% OUTPUT
%  ok     : (bool) search result
%  id     : (integer) variable_id (optional)
%
% USAGE
%  object.hasVariable(name)
%  hasVariable(object, name)
%

%%
function varargout = hasVariable(this, name)
    [ok, id] = ismember(name, this.EnsightVariableList(:,1));

    varargout{1} = ok;
  
    if nargout == 2
        varargout{2} = id;
    end
    
end