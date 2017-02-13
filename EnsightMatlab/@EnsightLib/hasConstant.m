%% hasConstant(object, name)
% Checks, whether a constant 'name' exists
%
% INPUT
%  object : |EnsightObj| object
%  name   : constant name (string)
%
% OUTPUT
%  ok     : (boolean) search result
%  id     : (integer) constant id (optional)
%
% USAGE
%  [ok, (id)] = object.hasConstant(name)
%

%%
function [varargout] = hasConstant(this, name)
    [ok, id] = ismember(name, this.Constants(:,1));
    varargout{1} = ok;
    if nargout == 2
        varargout{2} = id;
    end
end