%% interpolateVariable(object, position, variableName)
% Queries an interpolation of a variable for a single point
%
% INPUT
%  object        : |EnsightObj| object
%  position      : (vector,3x1) interpolation position
%  variableNames : (string) interpolation variable
%
% OUTPUT
%  result   : (double/vector) interpolation result
%  ok       : (bool) interpolation success
%
% USAGE
%  [result, ok] = object.interpolateVariable(position, variableName)
%

%%
function [result,ok] = interpolateVariable(this, position, variableName)

    if(~all(size(position)==[3,1]))
        error('Position has wrong dimensions. 3x1 needed.');
    end
    
    valueArray = [3, 2, 5, 0, 1];
    [tmp_result,tmp_ok] = EnsightLib_interface('obj', 'interpolate', valueArray, this.getObjectHandle(), position, variableName);
    
    if(tmp_ok==false)
        warning('EnsightLib::interpolateVariable: Interpolation failed. Point outside bounds.');
    end
    
    result = tmp_result;
    ok = tmp_ok;
end