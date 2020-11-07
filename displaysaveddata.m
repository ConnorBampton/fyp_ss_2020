clear all; close all;
normalise_flag = 0;
count = 1;
%https://www.mathworks.com/help/matlab/math/interpolating-scattered-data.html

% datafile = fopen('CoolTerm Capture 2020-10-28 12-33-50.txt');
% data = textscan(datafile, '%d %f %f %f %f %f','delimiter',' ', 'HeaderLines', 6);
% fclose (datafile);
sensor_data = importdata('CoolTerm Capture 2020-10-28 12-33-50.txt', ' ', 1);

% display(data2.data);
% display(sensor_data.data(1,:));

% values = [7,4,2,1,0,0,0,0,0,1,2,4,7];
% dataout = ones(13,45);
% input_data = 1;
fig_heatmap = figure;
clf
set(gcf,'Position',[100 100 1125 540], 'color', 'w');

fig_humidity = figure;
set(gcf,'Position',[1200 100 540 540], 'color', 'w');
    
while count <= size(sensor_data.data,1)
    % close all;
    
    % %Back of Heel BH
    % %Back Bridge BB
    % %Front Bridge FB
    % %Back of arch BA
    % %Front of arch FA
    % %Toes TO

    BH = sensor_data.data(count,6);
    BB = (sensor_data.data(count,5)+sensor_data.data(count,6))/2;
    FB = (sensor_data.data(count,5)+sensor_data.data(count,3))/2;
    BA = (sensor_data.data(count,4)+sensor_data.data(count,6))/2;
    FA = (sensor_data.data(count,4)+sensor_data.data(count,3))/2;
    TO = sensor_data.data(count,3);


    interpx = [6, 8,4,4,10,10,9, 5];
    interpy = [1, 1,14,33,14,33,40, 40];
    interpz = [BH,BH,BB,FB,BA,FA,TO,TO];


    %figure; 
    [xi,yi] = meshgrid(0:0.05:45, 0:0.05:13);
    zi = griddata(interpy,interpx,interpz,xi,yi);
    %surf(xi,yi,zi);
    %colormap hot
    %shading interp
    %view(322.5, 30);

    zmin = min(min(zi));
    zmax = max(max(zi));

    if (normalise_flag == 1)
        zi = (zi-zmin)./(zmax-zmin);
    end

    figure(fig_heatmap);
    t  = heatmap(zi, 'CellLabelColor', 'none');
    t.Colormap = hot;
    grid off
    ylim ([84 200]);
    xlim([24 804]);
    title("Temp Heatmap at Time = "+ num2str(sensor_data.data(count,1)));
    %Remove axis labels
    xdisp1 = t.XDisplayLabels;                                    % Current Display Labels
    t.XDisplayLabels = repmat(' ',size(xdisp1,1), size(xdisp1,2));   % Blank Display Labels
    ydisp1 = t.YDisplayLabels;                                    % Current Display Labels
    t.YDisplayLabels = repmat(' ',size(ydisp1,1), size(ydisp1,2));   % Blank Display Labels

    
    figure(fig_humidity);
    plot(sensor_data.data(1:count,1), sensor_data.data(1:count,2),'Marker', 'x', 'Color','r');
    grid on
    ylim([40 100]);
    title("Humidity from time 0 to time " + num2str(sensor_data.data(count,1)) + "  (Currently: " + num2str(sensor_data.data(count,2)) + "%)");
    
    count = count +1;
%     w = waitforbuttonpress;
    pause(1);
end