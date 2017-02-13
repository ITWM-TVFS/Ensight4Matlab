%% getConstant(object, name)
% Returns the value of a constant
%
% INPUT
%  object : |EnsightLib| object
%  name   : (string) constant name
%
% OUTPUT
%  value  : (numeric) constant value
%
% USAGE
%  value = object.getConstant(name)
%

%%
function value = getConstant(this, name)
    [~,id] = this.hasConstant(name);
    assert(id>0,'EnsightLib::getConstant - Constant does not exist. '); 
    value = this.Constants{id,2};
end