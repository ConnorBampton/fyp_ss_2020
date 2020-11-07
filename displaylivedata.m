clear all; close all;
normalise_flag = 0;
count = 1;

% Create unique filename based on date and time
filename = append('testdata ', datestr(now, 'mmmm dd  HH MM AM'), '.txt');


% Set up setial port and data structures
device = serialport("COM3", 115200);
device.Timeout = 1000;
humidity_data = [];
time_data = [];

% Open figures in correct positions / colours
fig_heatmap = figure;
clf
set(gcf,'Position',[100 100 1125 540], 'color', 'w');

fig_humidity = figure;
set(gcf,'Position',[1200 100 540 540], 'color', 'w');

% Initialize corner positions for heatmap surface
interpx = [6, 8,4,4,10,10,9, 5];
interpy = [1, 1,14,33,14,33,40, 40];
    
while 1
    % close all;

    % Wait for serial port to receive 38 string characters
    serial_read = read(device,38, "string");
    
    % Split data into arrays
    data_str = split(serial_read);
    sensor_data = str2double(data_str);
    time = sensor_data(1);
    time_data = [time_data, sensor_data(1)];
    humidity_data = [humidity_data, sensor_data(2)];
    
    % Save data into unique .txt file created at start of program
    fileID = fopen(filename, 'a');
    filwWRITE = fwrite(fileID, serial_read);
    fileCLOSE = fclose(fileID);
    
    %Generate corners to create heatmap surface from
    BH = sensor_data(6);
    BB = (sensor_data(5)+sensor_data(6))/2;
    FB = (sensor_data(5)+sensor_data(3))/2;
    BA = (sensor_data(4)+sensor_data(6))/2;
    FA = (sensor_data(4)+sensor_data(3))/2;
    TO = sensor_data(3);

    % Load these corners intoarray for surface fitting
    interpz = [BH,BH,BB,FB,BA,FA,TO,TO];


    % Create x and y data uing 0.05 spacing
    [xi,yi] = meshgrid(0:0.05:45, 0:0.05:13);
    
    % Meshgrid takes the data points and interpolates the z values accross
    % the smaller grid of xi and yi values
    zi = griddata(interpy,interpx,interpz,xi,yi);
    % Plot 3d surface
    %figure;
    %surf(xi,yi,zi);
    %colormap hot
    %shading interp
    %view(322.5, 30);

    % Normalise the data if wanted for comparison
    zmin = min(min(zi));
    zmax = max(max(zi));
    if (normalise_flag == 1)
        zi = (zi-zmin)./(zmax-zmin);
    end

    % Generate temperature heatmap, including all grid, colour and axis
    % options
    figure(fig_heatmap);
    t  = heatmap(zi, 'CellLabelColor', 'none');
    t.Colormap = hot;
    grid off
    ylim ([84 200]);
    xlim([24 804]);
    title("Temp Heatmap at Time = "+ num2str(time));
    %Remove axis labels
    xdisp1 = t.XDisplayLabels;                                    
    t.XDisplayLabels = repmat(' ',size(xdisp1,1), size(xdisp1,2));   
    ydisp1 = t.YDisplayLabels;                                    
    t.YDisplayLabels = repmat(' ',size(ydisp1,1), size(ydisp1,2));   

    
    % Generate humidity graph, including all grid, colour and axis
    % options
    figure(fig_humidity);
    %Only show up to the 50 most recent humidity points to stop the graph
    %from getting to small to view. changes (Can equate to up to ~100 mins)
    if count > 51
        plot(time_data(count-50:count), humidity_data(count-50:count),'Marker', 'x', 'Color','r');
        title("Humidity from time " + num2str(time_data(count-50)) + " to time " + num2str(time) + "  (Currently: " + num2str(sensor_data(2)) + "%)");
        xlim([time_data(count-50) time_data(count)]);
    else
        plot(time_data(1:count), humidity_data(1:count),'Marker', 'x', 'Color','r');
        title("Humidity from time " + num2str(time_data(1)) + " to time " + num2str(time) + "  (Currently: " + num2str(sensor_data(2)) + "%)");
        if count > 2
            xlim([time_data(1) time_data(count)]);
        end
    end
    grid on
    ylim([40 100]);
    title("Humidity from time " + num2str(time_data(1)) + " to time " + num2str(time) + "  (Currently: " + num2str(sensor_data(2)) + "%)");
    xlabel("Time (s)")
    ylabel("Relative Humidity (%)")
    count = count +1;
%     w = waitforbuttonpress;
    pause(1);
end